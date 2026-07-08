#include <SoftwareSerial.h>
String message = "";
SoftwareSerial Bluetooth(3, 2);

#define Test A5

#define LS_D0 A0
#define LS_D1 A1
#define LS_D2 A2
#define LS_D3 A3
#define LS_D4 A4

int latchPin = 8;  // Latch pin of 74HC595 is connected to Digital pin 5 (RCLK)
int dataPin = 9;   // Data pin of 74HC595 is connected to Digital pin 4 (SER)
int clockPin = 10;  // Clock pin of 74HC595 is connected to Digital pin 6 (SRCLK)

byte registersByte = 0;  // Variable to hold the pattern of which LEDs are currently turned on or off
String registersStr = "";

String currentInput = "";
String previousInput = "";
String displayInput = "#xx,xx,x,000,00,00,00#";
//                     0123456789111111111122
//                               012345678901



String digitSeg = "0000000";

String digitStr = "00000";

String str0 = "88888";
String str1 = "00000";
String str2 = "12345";
String str3 = "67890";


void setup() {
  Serial.begin(9600);
  Bluetooth.begin(9600);

  pinMode(Test, OUTPUT);
  digitalWrite(Test, HIGH);

  pinMode(LS_D0, OUTPUT);
  pinMode(LS_D1, OUTPUT);
  pinMode(LS_D2, OUTPUT);
  pinMode(LS_D3, OUTPUT);
  pinMode(LS_D4, OUTPUT);

  // Set all the pins of 74HC595 as OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  digitOff();
  Serial.println("Start");
}


void loop() {

  if (Bluetooth.available()) {
    char incomingChar = Bluetooth.read();
    if (incomingChar != '\n') {
      currentInput += String(incomingChar);
    } else currentInput = "";

    if (currentInput.length() == 22 && currentInput.substring(0, 1) == "#" && currentInput.substring(21, 22) == "#") {
      if (currentInput.substring(3, 4) == "," &&
          currentInput.substring(6, 7) == "," &&
          currentInput.substring(8, 9) == "," &&
          currentInput.substring(12, 13) == "," &&
          currentInput.substring(15, 16) == "," &&
          currentInput.substring(18, 19) == ",") {

        Serial.print("valid - ");
        Serial.println(currentInput);

        if (previousInput != currentInput) {
          displayInput = currentInput;
          previousInput = currentInput;
          Serial.println("UPDATED");
        }else Serial.println("input identical to previous");
      } else Serial.println("input invalid (002)");
    } else Serial.println("input invalid (001)");
  }


  digitStr = displayInput.substring(1, 3) + displayInput.substring(4, 6) + displayInput.substring(7, 8);
  for (int i = 0; i < 5; i++) {
    char d = digitStr[i];
    aaa(d - '0', i);
  }

}


void digitOff() {
  digitalWrite(LS_D0, LOW);
  digitalWrite(LS_D1, LOW);
  digitalWrite(LS_D2, LOW);
  digitalWrite(LS_D3, LOW);
  digitalWrite(LS_D4, LOW);
}


void updateShiftRegister() {
  digitalWrite(latchPin, LOW);

  registersStr = digitSeg + displayInput.substring(9, 12) + displayInput.substring(13, 15) + displayInput.substring(16, 18) + displayInput.substring(19, 21);

  //Serial.print(registersStr);

  for (int i = 0; i < 8; i++) {
    if (registersStr[i + 8] == '1') {
      bitSet(registersByte, i);
    } else bitClear(registersByte, i);
  }
  shiftOut(dataPin, clockPin, MSBFIRST, registersByte);

  for (int i = 0; i < 8; i++) {
    if (registersStr[i] == '1') {
      bitSet(registersByte, i);
    } else bitClear(registersByte, i);
  }
  shiftOut(dataPin, clockPin, MSBFIRST, registersByte);

  //Serial.println("");
  digitalWrite(latchPin, HIGH);
}


void aaa(int num, int digit) {
  digitOff();

  switch (num) {
    case 0:
      digitSeg = "1111110";
      break;
    case 1:
      digitSeg = "0110000";
      break;
    case 2:
      digitSeg = "1101101";
      break;
    case 3:
      digitSeg = "1111001";
      break;
    case 4:
      digitSeg = "0110011";
      break;
    case 5:
      digitSeg = "1011011";
      break;
    case 6:
      digitSeg = "1011111";
      break;
    case 7:
      digitSeg = "1110000";
      break;
    case 8:
      digitSeg = "1111111";
      break;
    case 9:
      digitSeg = "1111011";
      break;
    default:
      digitSeg = "0000001";
  }

  updateShiftRegister();

  switch (digit) {
    case 0:
      digitalWrite(LS_D0, HIGH);
      break;
    case 1:
      digitalWrite(LS_D1, HIGH);
      break;
    case 2:
      digitalWrite(LS_D2, HIGH);
      break;
    case 3:
      digitalWrite(LS_D3, HIGH);
      break;
    case 4:
      digitalWrite(LS_D4, HIGH);
      break;
  }

  delay(3);
}
