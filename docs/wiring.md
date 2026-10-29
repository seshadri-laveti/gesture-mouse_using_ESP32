# Air Gesture Mouse — Hardware Wiring Guide

This document explains how to connect the MPU6050 motion sensor and three push buttons to the ESP32 development board.

## 1. Components Required

- ESP32 development board
- MPU6050 accelerometer and gyroscope module
- Three push buttons
- Breadboard
- Jumper wires
- USB cable

## 2. MPU6050 Connections

The MPU6050 communicates with the ESP32 through the I2C interface.

| MPU6050 Pin | ESP32 Pin | Description |
|---|---|---|
| VCC | 3V3 | Power supply |
| GND | GND | Ground |
| SDA | GPIO 21 | I2C data |
| SCL | GPIO 22 | I2C clock |

The firmware uses I2C address `0x68`, which is typical when the MPU6050 AD0 pin is low.

## 3. Push Button Connections

Each push button connects between an ESP32 GPIO pin and GND.

| Button | ESP32 GPIO | Function |
|---|---|---|
| Button 1 | GPIO 25 | Left mouse click |
| Button 2 | GPIO 26 | Right mouse click |
| Button 3 | GPIO 27 | Toggle cursor movement |

The firmware configures these pins using `INPUT_PULLUP`.

When a button is released, the input reads HIGH. When pressed, it reads LOW.

No external pull-up resistors are required for these buttons.

## 4. Complete Connection Summary

```text
ESP32                  MPU6050
-----                  -------
3V3  ----------------> VCC
GND  ----------------> GND
GPIO 21 <------------> SDA
GPIO 22 <------------> SCL


ESP32                  PUSH BUTTONS
-----                  ------------
GPIO 25 -------------- Button 1 ---- GND
GPIO 26 -------------- Button 2 ---- GND
GPIO 27 -------------- Button 3 ---- GND
```

## 5. Operating Principle

1. The MPU6050 measures acceleration and angular velocity.
2. The ESP32 calculates roll and pitch angles.
3. The ESP32 sends motion data and button states through USB serial.
4. A Python application receives the serial data.
5. The Python application converts the motion data into mouse cursor movement.
6. Buttons 1 and 2 trigger left-click and right-click actions.
7. Button 3 toggles cursor movement on or off.

## 6. Troubleshooting

### MPU6050 is not detected

- Verify VCC and GND connections.
- Check SDA and SCL wiring.
- Confirm that the module uses I2C address `0x68`.
- Try an I2C scanner sketch.
- Check jumper wires and breadboard contacts.

### Buttons do not work

- Connect the other terminal of each button to GND.
- Confirm that the correct GPIO pins are used.
- Check that the firmware uses `INPUT_PULLUP`.

### Cursor does not move

- Confirm that the ESP32 firmware is uploaded.
- Verify that the Python program is running.
- Check the selected serial port.
- Close Arduino IDE Serial Monitor before running Python.
- Confirm that cursor movement is enabled using Button 3.

## 7. Safety Notes

- Check all connections before powering the circuit.
- Do not connect GPIO pins directly to the power supply.
- Use the ESP32 3.3 V supply for the MPU6050 module unless the specific module documentation states otherwise.
- Disconnect power before changing wiring.
