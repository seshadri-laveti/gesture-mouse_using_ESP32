```cpp
/*
  PROJECT: Air Gesture Mouse Using ESP32 and MPU6050

  Hardware:
    ESP32 development board
    MPU6050 accelerometer and gyroscope
    Button 1: Left click
    Button 2: Right click
    Button 3: Toggle cursor movement

  MPU6050 I2C:
    SDA -> GPIO 21
    SCL -> GPIO 22
    Address -> 0x68

  Buttons:
    Left click -> GPIO 25
    Right click -> GPIO 26
    Toggle cursor -> GPIO 27

  Serial output:
    roll_deg,pitch_deg,gx_dps,gy_dps,gz_dps,left_btn,right_btn,cursor_enabled

  Baud rate: 115200
*/

#include <Wire.h>
#include <math.h>

// ---------------- PIN CONFIGURATION ----------------

#define MPU_ADDR       0x68

#define SDA_PIN        21
#define SCL_PIN        22

#define LEFT_BUTTON    25
#define RIGHT_BUTTON   26
#define TOGGLE_BUTTON  27

// ---------------- TIMING ----------------

const unsigned long SEND_INTERVAL_MS = 20;
const unsigned long DEBOUNCE_MS = 40;

// ---------------- SENSOR DATA ----------------

int16_t axRaw, ayRaw, azRaw;
int16_t gxRaw, gyRaw, gzRaw;

float rollDeg = 0.0;
float pitchDeg = 0.0;

float gxDps = 0.0;
float gyDps = 0.0;
float gzDps = 0.0;

bool mpuAvailable = false;
bool cursorEnabled = true;

unsigned long lastSendTime = 0;

// ---------------- BUTTON DEBOUNCE ----------------

int previousToggleReading = HIGH;
int stableToggleReading = HIGH;

unsigned long lastToggleChange = 0;

// ---------------- MPU6050 REGISTER FUNCTION ----------------

void writeMPURegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// ---------------- INITIALIZE MPU6050 ----------------

bool initializeMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75);  // WHO_AM_I register

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(MPU_ADDR, 1) != 1) {
    return false;
  }

  uint8_t identity = Wire.read();

  if (identity != 0x68) {
    return false;
  }

  // Wake up the sensor
  writeMPURegister(0x6B, 0x00);

  // Accelerometer range: +/- 2g
  writeMPURegister(0x1C, 0x00);

  // Gyroscope range: +/- 250 degrees/second
  writeMPURegister(0x1B, 0x00);

  // Digital low-pass filter
  writeMPURegister(0x1A, 0x03);

  delay(100);

  return true;
}

// ---------------- READ MPU6050 ----------------

bool readMPU6050() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(MPU_ADDR, 14) != 14) {
    return false;
  }

  axRaw = (int16_t)((Wire.read() << 8) | Wire.read());
  ayRaw = (int16_t)((Wire.read() << 8) | Wire.read());
  azRaw = (int16_t)((Wire.read() << 8) | Wire.read());

  // Skip temperature bytes
  Wire.read();
  Wire.read();

  gxRaw = (int16_t)((Wire.read() << 8) | Wire.read());
  gyRaw = (int16_t)((Wire.read() << 8) | Wire.read());
  gzRaw = (int16_t)((Wire.read() << 8) | Wire.read());

  // Convert gyro values to degrees per second
  gxDps = gxRaw / 131.0;
  gyDps = gyRaw / 131.0;
  gzDps = gzRaw / 131.0;

  // Convert accelerometer readings to g
  float ax = axRaw / 16384.0;
  float ay = ayRaw / 16384.0;
  float az = azRaw / 16384.0;

  // Estimate orientation from gravity
  rollDeg = atan2(ay, az) * 180.0 / PI;

  pitchDeg = atan2(
    -ax,
    sqrt(ay * ay + az * az)
  ) * 180.0 / PI;

  return true;
}

// ---------------- UPDATE TOGGLE BUTTON ----------------

void updateToggleButton() {
  int reading = digitalRead(TOGGLE_BUTTON);

  if (reading != previousToggleReading) {
    lastToggleChange = millis();
  }

  if (millis() - lastToggleChange >= DEBOUNCE_MS) {
    if (reading != stableToggleReading) {
      stableToggleReading = reading;

      // Toggle only on a button press
      if (stableToggleReading == LOW) {
        cursorEnabled = !cursorEnabled;
      }
    }
  }

  previousToggleReading = reading;
}

// ---------------- SETUP ----------------

void setup() {
  Serial.begin(115200);

  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(TOGGLE_BUTTON, INPUT_PULLUP);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  delay(500);

  mpuAvailable = initializeMPU();

  if (mpuAvailable) {
    Serial.println("#MPU6050_READY");
  } else {
    Serial.println("#ERROR:MPU6050_NOT_FOUND");
  }
}

// ---------------- MAIN LOOP ----------------

void loop() {
  updateToggleButton();

  if (millis() - lastSendTime < SEND_INTERVAL_MS) {
    return;
  }

  lastSendTime = millis();

  // Attempt to reconnect if sensor is unavailable
  if (!mpuAvailable) {
    mpuAvailable = initializeMPU();

    if (!mpuAvailable) {
      return;
    }
  }

  if (!readMPU6050()) {
    mpuAvailable = false;
    return;
  }

  // Buttons are active LOW
  int leftButton =
    (digitalRead(LEFT_BUTTON) == LOW) ? 1 : 0;

  int rightButton =
    (digitalRead(RIGHT_BUTTON) == LOW) ? 1 : 0;

  // 1 means cursor movement enabled
  int cursorState = cursorEnabled ? 1 : 0;

  // Send exactly eight comma-separated values
  Serial.print(rollDeg, 2);
  Serial.print(",");

  Serial.print(pitchDeg, 2);
  Serial.print(",");

  Serial.print(gxDps, 2);
  Serial.print(",");

  Serial.print(gyDps, 2);
  Serial.print(",");

  Serial.print(gzDps, 2);
  Serial.print(",");

  Serial.print(leftButton);
  Serial.print(",");

  Serial.print(rightButton);
  Serial.print(",");

  Serial.println(cursorState);
}
```
