```python
"""
Air Gesture Mouse
ESP32 + MPU6050 + Three Push Buttons

Serial format:
roll_deg,pitch_deg,gx_dps,gy_dps,gz_dps,left_btn,right_btn,cursor_enabled

Usage:
python software/air_mouse.py --port COM5
"""

import argparse
import sys
import time

import serial
import pyautogui


# ---------------- CONFIGURATION ----------------

BAUD_RATE = 115200
SERIAL_TIMEOUT = 0.1

# Adjust these values to change cursor sensitivity.
SENSITIVITY_X = 2.0
SENSITIVITY_Y = 2.0

# Ignore small movements to reduce unwanted cursor motion.
DEAD_ZONE = 1.5

# Limit cursor speed.
MAX_STEP = 35

# Set to True to invert the horizontal or vertical direction.
INVERT_X = False
INVERT_Y = False

pyautogui.FAILSAFE = True
pyautogui.PAUSE = 0.01


# ---------------- HELPER FUNCTIONS ----------------

def clamp(value, minimum, maximum):
    """Limit a value to a specified range."""
    return max(minimum, min(value, maximum))


def calculate_step(angle, sensitivity, invert=False):
    """Convert sensor angle into cursor movement."""
    if abs(angle) < DEAD_ZONE:
        return 0

    movement = angle * sensitivity

    if invert:
        movement = -movement

    return int(clamp(movement, -MAX_STEP, MAX_STEP))


def parse_sensor_data(line):
    """
    Parse the eight comma-separated fields sent by the ESP32.
    Returns None if the line is invalid.
    """
    try:
        values = [item.strip() for item in line.split(",")]

        if len(values) != 8:
            return None

        roll = float(values[0])
        pitch = float(values[1])
        gx = float(values[2])
        gy = float(values[3])
        gz = float(values[4])

        left_button = int(values[5])
        right_button = int(values[6])
        cursor_enabled = int(values[7])

        if left_button not in (0, 1):
            return None

        if right_button not in (0, 1):
            return None

        if cursor_enabled not in (0, 1):
            return None

        return (
            roll,
            pitch,
            gx,
            gy,
            gz,
            left_button,
            right_button,
            cursor_enabled,
        )

    except (ValueError, TypeError):
        return None


# ---------------- MAIN PROGRAM ----------------

def main():
    parser = argparse.ArgumentParser(
        description="Control the computer mouse using an ESP32 and MPU6050."
    )

    parser.add_argument(
        "--port",
        required=True,
        help="Serial port of the ESP32, for example COM5.",
    )

    parser.add_argument(
        "--baud",
        type=int,
        default=BAUD_RATE,
        help="Serial baud rate (default: 115200).",
    )

    args = parser.parse_args()

    try:
        connection = serial.Serial(
            port=args.port,
            baudrate=args.baud,
            timeout=SERIAL_TIMEOUT,
        )

    except serial.SerialException as error:
        print(f"Could not open serial port {args.port}: {error}")
        print("Check the COM port and close Arduino Serial Monitor.")
        sys.exit(1)

    print("Air Gesture Mouse started.")
    print(f"Connected to ESP32 on {args.port}.")
    print("Press Ctrl+C to stop.")
    print("Move your hand to control the cursor.")
    print("Keep the pointer away from the screen corners.")
    print()

    # Reset serial input so stale data is discarded.
    connection.reset_input_buffer()

    previous_left = 0
    previous_right = 0

    try:
        while True:
            raw_line = connection.readline()

            if not raw_line:
                continue

            try:
                line = raw_line.decode("utf-8", errors="ignore").strip()
            except UnicodeDecodeError:
                continue

            if not line or line.startswith("#"):
                continue

            data = parse_sensor_data(line)

            if data is None:
                continue

            (
                roll,
                pitch,
                gx,
                gy,
                gz,
                left_button,
                right_button,
                cursor_enabled,
            ) = data

            # Move cursor only when enabled.
            if cursor_enabled == 1:
                dx = calculate_step(
                    roll,
                    SENSITIVITY_X,
                    INVERT_X,
                )

                dy = calculate_step(
                    pitch,
                    SENSITIVITY_Y,
                    INVERT_Y,
                )

                if dx != 0 or dy != 0:
                    try:
                        pyautogui.moveRel(
                            dx,
                            dy,
                            duration=0,
                        )
                    except pyautogui.FailSafeException:
                        print("PyAutoGUI safety stop activated.")
                        break

            # Perform a click only when the button changes
            # from released (0) to pressed (1).
            if left_button == 1 and previous_left == 0:
                pyautogui.click(button="left")
                print("Left click")

            if right_button == 1 and previous_right == 0:
                pyautogui.click(button="right")
                print("Right click")

            previous_left = left_button
            previous_right = right_button

    except KeyboardInterrupt:
        print("\nStopping Air Gesture Mouse.")

    except serial.SerialException as error:
        print(f"Serial communication error: {error}")

    finally:
        connection.close()
        print("Serial port closed.")


if __name__ == "__main__":
    main()
```
