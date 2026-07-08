# Digital Baseball Scoreboard Controll with Mobile App

A high-performance, bare-metal C implementation for the ATmega328P microcontroller driving an electronic baseball scoreboard display. This project updates an initial Arduino-based sketch into optimized, non-blocking AVR C by shifting tasks like 7-segment multiplexing and Bluetooth UART data packet parsing out of the main running loop and directly into hardware interrupts.

## 🚀 Key Improvements Over Original Arduino Implementation
* **Zero CPU Lag Multiplexing:** Replaced the freezing `delay(3)` multiplexer loops with an automated **Timer0 interrupt** firing every ~2ms. The display updates seamlessly in the background without affecting processing speeds.
* **Non-Blocking Serial Pipeline:** Switched the original polled `SoftwareSerial` reading loop over to a hardwired native **USART Receive Complete Interrupt (`USART_RX_vect`)**.
* **Streamlined Memory Footprint:** Erased the dynamic Arduino `String` libraries—known to cause dangerous heap memory fragmentation—and replaced them with rigid, fixed-length native C character buffers (`char[]`) and fast bitwise data slicing operations.
* **Instant Inline Verification:** The serial listener includes an integrated live packet state-machine parser that instantly accepts or dumps incoming packages on-the-fly according to structural rules before allowing bad data to corrupt active score registers.

---

## 🛠 Hardware Circuit Pin Configurations

### 1. 7-Segment Multiplexer Selectors (Common Anode/Cathode Control)
The 5 physical scoreboard character digital windows track directly into native AVR **Port C**:
* **Digit 0** (Home Left) $\rightarrow$ Pin `PC0` (Arduino A0)
* **Digit 1** (Home Right) $\rightarrow$ Pin `PC1` (Arduino A1)
* **Digit 2** (Away Left) $\rightarrow$ Pin `PC2` (Arduino A2)
* **Digit 3** (Away Right) $\rightarrow$ Pin `PC3` (Arduino A3)
* **Digit 4** (Inning Window) $\rightarrow$ Pin `PC4` (Arduino A4)

### 2. 74HC595 Cascaded Shift Registers
The scoreboard leverages dual cascaded 8-bit shift registers wired into native AVR **Port B** to limit required microcontroller pinout:
* **Latch (RCLK)** $\rightarrow$ Pin `PB0` (Arduino Digital D8)
* **Data (SER)** $\rightarrow$ Pin `PB1` (Arduino Digital D9)
* **Clock (SRCLK)** $\rightarrow$ Pin `PB2` (Arduino Digital D10)

### 3. Diagnostic Testing & Telemetry Ingestion
* **Status Pin:** Wired to Pin `PC5` (Arduino A5) for operational visibility.
* **Hardware UART RX:** Connected to Pin `PD0` (Arduino RX<-0) to read wireless commands at **9600 Baud**.

---

## 📬 Bluetooth Data Packet Format
The system relies on a rigid, 22-character telemetry frame demarcated by `#` framing anchors and structural commas:

```text
 # x x , x x , x , 0 0 0 , 0 0 , 0 0 , 0 0 #
 | \ /   \ /   |   \___/   \________________/
 |  |     |    |     |             |
 | Home  Away Inning |      Auxiliary Flags
Start                |      (Balls, Strikes, Outs, etc.)
                   Segment Bit-9
