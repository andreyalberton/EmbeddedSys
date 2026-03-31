# Monitoring System 

**Module:** 6231COMP - Embedded Systems  

## Project Overview
This project is an embedded system designed to monitor the environment of a CNC machine. It reads temperature and humidity data, logs historical peaks, and provides real-time alerts via an RGB LED, buzzer, and an TFT display. The program has features like = operational modes, including a safety lockout mechanism and an user mode controlled with the rotary encoder.

## Features Implemented
This repository contains the completed implementation for the following core specification features:
* **Feature A (Basic Loop):** Low-latency sensor reading with status indication.
* **Feature B (Data & Serial Logging):** 240-record volatile memory buffer (2-hour peak tracking) and 5-second serial debug outputs.
* **Feature C (Sounder & Output Display):** Integration of buzzer alerts and a live TFT dashboard showing current metrics, 2-hour peaks, and system status.
* **Feature D (Inspection, Auto Lock-Out & Mode Selection):** Stateful mode management (`STD`, `HIGH_SENS`, `LOCKOUT`) via hardware interrupts/debounced buttons, including a 30-second continuous unsafe lockout trap.
* **Feature E (User Editable Mode):** A dedicated UI screen allowing users to dial in custom Min/Max thresholds (0-50°C, 0-100%) using a Digilent PmodENC rotary encoder with strict phase-glitch and edge-detection debouncing.

## Hardware Requirements
* **MCU:** ESP32 Development Board
* **Sensors:** DHT11 (Temperature & Humidity), LDR (Photoresistor)
* **Display:** 1.8" TFT SPI Screen (ST7735)
* **Inputs:** * Digilent PmodENC Rotary Encoder (with integrated push-button)
  * 2x Tactile Push Buttons (Mode Switch & Stop/Reset)
* **Outputs:** Common Cathode RGB LED, Active Buzzer

## Software & Dependencies
[cite_start]This project was built using **PlatformIO** on VSCode[cite: 28]. It heavily utilizes Object-Oriented C++ principles to decouple hardware polling from application logic (e.g., `DisplayManager`, `ModeManager`, `EditManager`).

**Required Libraries:**
* `Adafruit GFX Library`
* `Adafruit ST7735 and ST7789 Library`
* *(Add your DHT sensor library here, e.g., `DHT sensor library by Adafruit`)*

## Application Entry & Compilation
As per the project specification, the application entry point is located in `src/main.cpp`. 
* **`setup()`:** Initializes serial communication, hardware pin modes, and boots all manager classes.
* **`loop()`:** Executes the cyclic executive, delegating hardware polling to manager classes, handling instant UI overrides for the rotary encoder, and running the 2-second sensor reading timer.

### Compile Instructions
1. Open the project folder in **VSCode** with the **PlatformIO** extension installed.
2. Ensure your ESP32 is connected via USB.
3. Click the **PlatformIO: Build** button (the checkmark `✓` in the bottom toolbar) to compile the firmware.
4. Click the **PlatformIO: Upload** button (the right arrow `→`) to flash the ESP32.
5. Open the Serial Monitor (plug icon) and set the baud rate to `115200` to view the 5-second debug logs.

## 🎮 Usage Guide
* **Standard Operation:** The TFT will display live readings. If limits are exceeded, the buzzer and LED will alert you.
* **Change Modes:** Press the Mode button (Pin 32) to swap between Standard and High-Sensitivity limits.
* **Clear Lockout:** If the system enters Lockout mode, press and hold the Stop button (Pin 33).
* **Set Custom Limits:** Click the Rotary Encoder knob to enter `USER_CUSTOM` mode. Turn the knob to adjust the boundaries, and click to cycle through the settings.