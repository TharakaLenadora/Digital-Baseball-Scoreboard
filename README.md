# ⚾ Digital Baseball Scoreboard with Mobile App Control

A real-time embedded baseball scoreboard built using the **ATmega328P microcontroller** and **Embedded C**, designed to display live game scores received from a companion Android application.

<p align="center">

![Platform](https://img.shields.io/badge/Platform-ATmega328P-blue)
![Language](https://img.shields.io/badge/Language-Embedded%20C-success)
![IDE](https://img.shields.io/badge/IDE-Microchip%20Studio-orange)
![Communication](https://img.shields.io/badge/Communication-UART-blueviolet)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen)
![License](https://img.shields.io/badge/License-Educational-lightgrey)

</p>

---

# 📖 Overview

The **Digital Baseball Scoreboard with Mobile App Control** is an embedded systems project that provides a real-time electronic scoreboard for baseball games.

The system consists of two independent components:

- 📱 **Android Mobile Application** (Companion App) for score management.
- ⚙️ **ATmega328P Embedded Scoreboard** that receives score data and updates the LED display in real time.

The Android application sends score updates through a **Bluetooth (HC-05)** connection. The HC-05 communicates with the ATmega328P using **UART serial communication**, allowing the microcontroller to process incoming data and update the scoreboard.

The display is driven using **74HC595 shift registers** with multiplexing to efficiently control multiple seven-segment displays while minimizing I/O usage.

---

# 🚀 Project Highlights

- Designed using **ATmega328P**
- Written entirely in **Embedded C**
- Register-level AVR programming
- UART-based communication
- Compatible with HC-05 Bluetooth module
- Multiplexed LED display
- Interrupt-driven architecture
- Low-cost hardware implementation
- Real-time score updating
- Educational embedded systems project

---

# ✨ Key Features

- ⚾ Live baseball score display
- 📱 Mobile application control
- 📡 UART serial communication
- 📶 HC-05 Bluetooth compatible
- 🔄 Multiplexed seven-segment display
- ⏱ Timer interrupt display refreshing
- ⚡ Efficient register-level programming
- 💡 Low hardware cost
- 🖥 Portable electronic scoreboard

---

# 🏗 System Architecture

```text
            Android Mobile Application
                       │
                Bluetooth (HC-05)
                       │
                  UART Interface
                       │
                ATmega328P MCU
                       │
      ┌────────────────┼────────────────┐
      │                                 │
 UART Receive ISR               Timer Interrupt
      │                                 │
      └────────────────┼────────────────┘
                       │
                Score Processing
                       │
                Display Buffer
                       │
                 74HC595 Drivers
                       │
        Multiplexed 7-Segment Displays
```

---

# 🛠 Hardware Components

| Component | Purpose |
|------------|---------|
| ATmega328P | Main Microcontroller |
| HC-05 Bluetooth Module | Wireless Communication |
| 74HC595 Shift Registers | LED Display Driver |
| Seven-Segment Displays | Score Display |
| 16 MHz Crystal Oscillator | System Clock |
| Current Limiting Resistors | LED Protection |
| 5V Power Supply | System Power |

---

# 🛠 Technologies Used

| Category | Technology |
|----------|------------|
| Programming Language | Embedded C |
| Microcontroller | ATmega328P |
| IDE | Microchip Studio |
| Compiler | AVR-GCC |
| Communication | UART |
| Wireless Module | HC-05 Bluetooth |
| Display Driver | 74HC595 |
| Version Control | Git & GitHub |

---

# 📂 Repository Structure

```text
Digital-Baseball-Scoreboard-with-Mobile-App-Control/

├── Main.c
├── Makefile
└── README.md
```

---

# ⚙ Hardware Connections

## UART

| ATmega328P | Function |
|------------|----------|
| PD0 | RX |
| PD1 | TX |

---

## 74HC595

| ATmega328P | Function |
|------------|----------|
| PB0 | Latch |
| PB1 | Data |
| PB2 | Clock |

---

## Display Selection

| ATmega328P | Display |
|------------|----------|
| PC0 | Digit 1 |
| PC1 | Digit 2 |
| PC2 | Digit 3 |
| PC3 | Digit 4 |
| PC4 | Digit 5 |

---

# 📡 Communication

The Android application sends scoreboard information through the HC-05 Bluetooth module.

The HC-05 forwards the received data to the ATmega328P using **UART serial communication**.

After receiving a complete data packet, the microcontroller updates the display immediately.

Example packet:

```text
#00,00,0,000,00,00,00#
```

---

# 🔄 Program Flow

```text
Power On
    │
Initialize MCU
    │
Initialize UART
    │
Initialize Timer
    │
Enable Interrupts
    │
Receive UART Data
    │
Decode Score Packet
    │
Update Display Buffer
    │
Refresh Display
    │
Repeat
```

---

# 🧠 Embedded Concepts Demonstrated

- Embedded C Programming
- AVR Register Programming
- UART Communication
- Interrupt Service Routines (ISR)
- Timer Configuration
- Display Multiplexing
- Shift Register Interfacing
- Bitwise Operations
- GPIO Programming
- Real-Time Embedded Systems

---

# 📷 Project Images

Add screenshots inside an **images/** folder.

```
images/
│
├── prototype.jpg
├── scoreboard.jpg
├── circuit.png
├── pcb.jpg
└── wiring.jpg
```

Example:

```markdown
![Prototype](images/prototype.jpg)
```

---

# 🎥 Project Report & Demonstration

The complete project report, presentation, and demonstration videos are available here:

🔗 https://lnkd.in/gK_EZh8J

---

# 📱 Companion Android Application

This scoreboard is designed to work with the **Baseball Player Management & Live Scoring Mobile Application**.

Features include:

- Player Management
- Live Score Updates
- Match Statistics
- Team Management
- Coach Dashboard
- Admin Dashboard
- Firebase Authentication
- Cloud Database

Repository:

https://github.com/TharakaLenadora/Baseball-Player-Management-Mobile-App

---

# 🚀 Future Improvements

- Wi-Fi Connectivity
- Cloud Synchronization
- Live Streaming Integration
- Remote Score Updates
- Match Analytics Dashboard
- Real-Time Clock
- EEPROM Backup
- OLED Configuration Display
- Mobile Notifications

---

# 🎓 Educational Purpose

This project was developed as part of the **B.Sc. (Hons) Electronics and Information Technology** degree at the **University of Colombo**.

It demonstrates practical knowledge in:

- Embedded Systems
- AVR Microcontroller Programming
- Digital Electronics
- Serial Communication
- Mobile-to-Hardware Integration
- Real-Time Embedded Applications


---

⭐ If you found this project useful, consider giving the repository a star!
