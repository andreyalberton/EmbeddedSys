# CNC Machine Monitoring System

**Module:** 6231COMP - Embedded Systems

## Project Overview

An embedded monitoring system for a CNC machine environment. It reads temperature and humidity from a DHT11 sensor, logs data into a 24-hour circular buffer, and gives real-time feedback through an RGB LED, piezo buzzer, and a TFT display. The system supports multiple operating modes, a safety lockout mechanism, and user-editable thresholds via a rotary encoder.

## Features

* **Feature A (Basic Loop):** Non-blocking sensor reading every 2 seconds with status indication.
* **Feature B (Data & Serial Logging):** 2880-record circular buffer (24 hours at 30-second intervals) with 2-hour peak tracking and 5-second serial debug output.
* **Feature C (Sounder & Output Display):** Buzzer alerts and a live TFT dashboard showing current readings, 2-hour peaks, and system status.
* **Feature D (Inspection, Auto Lock-Out & Mode Selection):** Four system modes (`STD`, `HIGH_SENS`, `INSPECTION`, `LOCKOUT`). Single press toggles STD/HIGH_SENS, double-click toggles Inspection mode, 5-second hold clears Lockout. Lockout triggers when 2+ unsafe episodes exceed 5 minutes cumulative within a 15-minute rolling window.
* **Feature E (User Editable Mode):** Edit screen for setting custom Min/Max thresholds using the rotary encoder.

## Hardware

* **MCU:** ESP32-DevKitS 
* **Sensors:** DHT11 (Temperature & Humidity) on GPIO 4, LDR (Photoresistor) on GPIO 34
* **Display:** Adafruit 1.44" Color TFT (ST7735) via VSPI
* **Input:** Digilent PmodENC Rotary Encoder (CLK GPIO 26, DT GPIO 27, SW GPIO 14), Mode Button on GPIO 32
* **Output:** Common-cathode RGB LED (R GPIO 15, G GPIO 21, B GPIO 19), Piezo Buzzer on GPIO 25, TFT Backlight on GPIO 22

## Software & Dependencies

Built with **PlatformIO** on VSCode. The firmware uses an OOP architecture with separate Manager classes (`SensorManager`, `DisplayManager`, `ModeManager`, `AlertManager`, `EditManager`) to keep hardware polling separate from application logic.

**Required Libraries:**

* `Adafruit GFX Library`
* `Adafruit ST7735 and ST7789 Library`
* `DHT sensor library by Adafruit`

## Building and Flashing

Entry point is `src/main.cpp`.

* **`setup()`** initialises serial at 115200, sets up pin modes, and boots all Manager classes.
* **`loop()`** runs a cyclic order using `millis()` — no `delay()` calls. It polls buttons and the encoder every iteration, reads sensors every 2 seconds, logs data every 30 seconds, and prints debug info every 5 seconds.

### Steps

1. Open the project in VSCode with the PlatformIO extension.
2. Connect your ESP32 via USB.
3. Click **Build**  to compile.
4. Click **Upload**  to flash the board.
5. Open the Serial Monitor and set baud to `115200` to see debug output.

## Usage

* **Normal operation:** The TFT shows live temperature, humidity, mode, and safety status. Green LED means all clear.
* **Change mode:** Short press the Mode Button (GPIO 32) to swap between Standard and High Sensitivity.
* **Inspection mode:** Double-click the Mode Button to pause all threshold checking. Double-click again to resume.
* **Lockout:** If the system locks out, press and hold the Mode Button for 5 seconds to clear it.
* **Custom thresholds:** Click the encoder knob to enter edit mode. Turn to adjust values, click to cycle through Temp Min, Temp Max, Hum Min, Hum Max. After the last field it saves and activates User Mode.
