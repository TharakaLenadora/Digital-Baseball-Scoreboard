#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

// ==========================================
// 1. CONFIGURATION & PIN DEFINITIONS
// ==========================================

// Test Pin: Arduino A5 -> PC5
#define TEST_DDR   DDRC
#define TEST_PORT  PORTC
#define TEST_PIN   PC5

// Digit Select Pins: Arduino A0 to A4 -> PC0 to PC4
#define DIGIT_DDR   DDRC
#define DIGIT_PORT  PORTC
#define DIGIT_MASK  (0x1F) // Pins PC0, PC1, PC2, PC3, PC4

// Shift Register 74HC595 Pins
// Latch (RCLK): Arduino D8  -> PB0
// Data (SER):    Arduino D9  -> PB1
// Clock (SRCLK): Arduino D10 -> PB2
#define SHIFT_DDR   DDRB
#define SHIFT_PORT  PORTB
#define LATCH_PIN   PB0
#define DATA_PIN    PB1
#define CLOCK_PIN   PB2

#define PACKET_LEN 22

// ==========================================
// 2. GLOBAL VOLATILE VARIABLES (Interrupt Safe)
// ==========================================
static volatile char rx_packet[PACKET_LEN + 1];
static volatile uint8_t packet_ready = 0;

static volatile char display_packet[PACKET_LEN + 1] = "#00,00,0,000,00,00,00#";
static volatile uint8_t current_digit = 0;

// 7-Segment common cathode lookup map (0-9)
static const uint8_t seg_map[10] = {
    0b11111100, // 0
    0b01100000, // 1
    0b11011010, // 2
    0b11110010, // 3
    0b01100110, // 4
    0b10110110, // 5
    0b10111110, // 6
    0b11100000, // 7
    0b11111110, // 8
    0b11110110  // 9
};
#define SEG_DEFAULT 0b00000010 // Default '-' pattern if character is invalid

// ==========================================
// 3. LOW-LEVEL SUBSYSTEM DRIVERS
// ==========================================

void uart_init(uint32_t baud) {
    uint16_t ubrr = F_CPU / 16 / baud - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // Enable Rx, Tx, and Rx Complete Interrupt
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);              // 8-bit data, 1 stop bit
}

uint8_t uart_has_new_data(void) {
    return packet_ready;
}

void uart_get_packet(char* buffer) {
    cli(); // Atomic block protection while copying shared buffer
    strcpy(buffer, (char*)rx_packet);
    packet_ready = 0;
    sei();
}

void shift_init(void) {
    SHIFT_DDR |= (1 << LATCH_PIN) | (1 << DATA_PIN) | (1 << CLOCK_PIN);
}

// Emulates fast byte shifting down the register cascade
static void shift_byte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        SHIFT_PORT &= ~(1 << CLOCK_PIN);
        if (byte & (1 << i)) {
            SHIFT_PORT |= (1 << DATA_PIN);
        } else {
            SHIFT_PORT &= ~(1 << DATA_PIN);
        }
        SHIFT_PORT |= (1 << CLOCK_PIN);
    }
}

void shift_out_data(uint8_t seg_pattern, char* score_data) {
    // Replicates the exact parsing behavior of updateShiftRegister()
    // Byte 1: segment pattern bits mixed with data character index 9
    uint8_t reg1 = (seg_pattern & 0xFE) | ((score_data[9] == '1') ? 1 : 0);
    
    // Byte 2: parses out state checks for string positions [10, 11, 13, 14, 16, 17, 19, 20]
    uint8_t reg2 = 0;
    if (score_data[10] == '1') reg2 |= (1 << 7);
    if (score_data[11] == '1') reg2 |= (1 << 6);
    if (score_data[13] == '1') reg2 |= (1 << 5);
    if (score_data[14] == '1') reg2 |= (1 << 4);
    if (score_data[16] == '1') reg2 |= (1 << 3);
    if (score_data[17] == '1') reg2 |= (1 << 2);
    if (score_data[19] == '1') reg2 |= (1 << 1);
    if (score_data[20] == '1') reg2 |= (1 << 0);

    SHIFT_PORT &= ~(1 << LATCH_PIN);
    shift_byte(reg2); // Sends out trailing scoreboard variables
    shift_byte(reg1); // Sends out current multiplexed segment data
    SHIFT_PORT |= (1 << LATCH_PIN);
}

void display_init(void) {
    DIGIT_DDR |= DIGIT_MASK;   // Multiplexer selectors set to outputs
    DIGIT_PORT &= ~DIGIT_MASK; // Pull pins off initially
    
    // Setup Timer0: CTC Mode, Prescaler 1/1024. Clear Timer on Match.
    // Triggers visual updates smoothly at ~61Hz across all 5 digits combined
    TCCR0A = (1 << WGM01); 
    TCCR0B = (1 << CS02) | (1 << CS00); 
    OCR0A = 250;
    TIMSK0 |= (1 << OCIE0A); // Enable compare match interrupt
}

void display_update_buffer(char* packet) {
    cli();
    strcpy((char*)display_packet, packet);
    sei();
}

// ==========================================
// 4. INTERRUPT SERVICE ROUTINES (ISRs)
// ==========================================

// UART Packet Evaluator (Live Packet State Machine Parser)
ISR(USART_RX_vect) {
    char c = UDR0;
    static char buf[PACKET_LEN + 1];
    static uint8_t idx = 0;

    if (c == '\n') {
        idx = 0;
        return;
    }

    if (idx < PACKET_LEN) {
        buf[idx++] = c;
    } else {
        idx = 0; // Overflow safety net 
    }

    // Validate the complete block structure immediately on arrival
    if (idx == PACKET_LEN) {
        buf[PACKET_LEN] = '\0';
        if (buf[0] == '#' && buf[21] == '#' &&
            buf[3] == ',' && buf[6] == ',' && buf[8] == ',' &&
            buf[12] == ',' && buf[15] == ',' && buf[18] == ',') {
            
            strcpy((char*)rx_packet, buf);
            packet_ready = 1;
        }
        idx = 0;
    }
}

// Background Display Driver Interrupt (Runs every ~16ms per digit)
ISR(TIMER0_COMPA_vect) {
    // 1. Instantly pull digit selectors off to avoid visual ghosting trails
    DIGIT_PORT &= ~DIGIT_MASK;

    // 2. Replicates the character substring selection parsing logic 
    char raw_char = ' ';
    switch(current_digit) {
        case 0: raw_char = display_packet[1];  break; // Digit 0 -> Substring index 1
        case 1: raw_char = display_packet[2];  break; // Digit 1 -> Substring index 2
        case 2: raw_char = display_packet[4];  break; // Digit 2 -> Substring index 4
        case 3: raw_char = display_packet[5];  break; // Digit 3 -> Substring index 5
        case 4: raw_char = display_packet[7];  break; // Digit 4 -> Substring index 7
    }

    uint8_t num = raw_char - '0';
    uint8_t pattern = (num >= 0 && num <= 9) ? seg_map[num] : SEG_DEFAULT;

    // 3. Drive current bit map out to hardware cascaded registers
    shift_out_data(pattern, (char*)display_packet);

    // 4. Set targeted anode/cathode line high to activate display element 
    DIGIT_PORT |= (1 << current_digit);

    // 5. Index cycling logic
    current_digit++;
    if (current_digit >= 5) current_digit = 0;
}

// ==========================================
// 5. MAIN LOOP ENTRY
// ==========================================

int main(void) {
    // Setup and enable system status debugging pin
    TEST_DDR |= (1 << TEST_PIN);
    TEST_PORT |= (1 << TEST_PIN);

    // Hardware subsystems configurations
    uart_init(9600);
    shift_init();
    display_init();

    // Enable processing exceptions globally
    sei();

    char active_packet[PACKET_LEN + 1] = "#00,00,0,000,00,00,00#";

    while (1) {
        // Non-blocking telemetry ingestion evaluation
        if (uart_has_new_data()) {
            uart_get_packet(active_packet);
            display_update_buffer(active_packet);
        }

        // Your main loop is 100% free of processing delays.
        // You can run game state math, button detection, or buzzers here safely!
    }

    return 0; 
}