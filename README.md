# Air Gesture Mouse Using ESP32

## Project Overview

The Air Gesture Mouse is an embedded systems project that allows users to control a computer mouse using hand movements. It uses an ESP32 microcontroller, an MPU6050 accelerometer and gyroscope, three push buttons, and Python software.

The MPU6050 detects hand orientation and movement. The ESP32 sends sensor data to a computer through serial communication, and a Python application converts the data into mouse cursor movements and clicks.

## Features

- Touch-free mouse cursor movement.
- Hand motion detection using MPU6050.
- Left-click control using a push button.
- Right-click control using a push button.
- Cursor movement enable/disable button.
- ESP32 firmware developed using Arduino IDE.
- Python-based computer mouse control.
- Serial communication between ESP32 and computer.

## Hardware Requirements

- ESP32 development board
- MPU6050 accelerometer and gyroscope module
- Three push buttons
- Breadboard
- Jumper wires
- USB cable
- Computer with Python installed

## Hardware Connections

### MPU6050 to ESP32

| MPU6050 Pin | ESP32 Pin |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### Push Buttons to ESP32

| Button | ESP32 Pin | Function |
|---|---|---|
| Button 1 | GPIO 25 | Left click |
| Button 2 | GPIO 26 | Right click |
| Button 3 | GPIO 27 | Enable/disable cursor movement |

Connect the other terminal of each button to GND. The firmware uses the ESP32 internal pull-up resistors.

## Software Requirements

- Arduino IDE
- ESP32 board support for Arduino IDE
- Python 3
- PySerial
- PyAutoGUI

## Installation

### Step 1: Upload the ESP32 Firmware

1. Open `firmware/air_gesture_mouse.ino` in Arduino IDE.
2. Select the appropriate ESP32 board.
3. Connect the ESP32 to your computer.
4. Select the correct serial port.
5. Upload the firmware.

### Step 2: Install Python Dependencies

Open a terminal in the project directory and run:

`python -m pip install -r requirements.txt`

### Step 3: Run the Python Application

Close Arduino IDE Serial Monitor before starting the Python application.

Run:

`python software/air_mouse.py --port COM5`

Replace `COM5` with the actual serial port assigned to your ESP32.

## Working Principle

1. The MPU6050 measures hand movement and orientation.
2. The ESP32 reads the sensor data.
3. The ESP32 transmits the data and button states over USB serial.
4. Python reads and processes the incoming data.
5. The mouse cursor moves according to the sensor readings.
6. The push buttons provide left-click, right-click, and cursor enable/disable functions.

## Serial Data Format

The firmware sends eight comma-separated values:

`roll_deg,pitch_deg,gx_dps,gy_dps,gz_dps,left_btn,right_btn,toggle_btn`

The serial communication baud rate is 115200.

## Applications

- Touch-free computer interaction
- Human-computer interaction
- Embedded systems education
- Motion-controlled interfaces
- Assistive technology prototypes

## Future Enhancements

- Improved sensor calibration
- Motion filtering for smoother cursor movement
- Adjustable cursor sensitivity
- Wireless communication
- Custom gesture recognition
- Additional mouse functions


## Author

Developed as an educational embedded systems project using ESP32, MPU6050, and Python.
