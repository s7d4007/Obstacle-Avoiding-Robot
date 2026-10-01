// Final Modified Working Code
// Last Modified : 23-July-2025
// Time : 3:38 P.M.

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AFMotor.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Custom Characters
byte arrowUp[8]    = {0b00100, 0b01110, 0b10101, 0b00100, 0b00100, 0b00100, 0b00100, 0b00000};
byte arrowDown[8]  = {0b00100, 0b00100, 0b00100, 0b00100, 0b10101, 0b01110, 0b00100, 0b00000};
byte arrowLeft[8]  = {0b00100, 0b01000, 0b11111, 0b01000, 0b00100, 0b00000, 0b00000, 0b00000};
byte arrowRight[8] = {0b00100, 0b00010, 0b11111, 0b00010, 0b00100, 0b00000, 0b00000, 0b00000};
byte fireChar[8]   = {0b00100, 0b01010, 0b00100, 0b01110, 0b10101, 0b00100, 0b00000, 0b00000};

// IR Sensors
const int irSensorLeftPin = A1;
const int irSensorRightPin = A2;

// Fire Sensor
const int fireSensorAnalogPin = A0;
const int fireSensorAnalogThreshold = 400;

// Buzzer
const int buzzerPin = A3;

// Motors
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// Speeds
const int motorSpeed = 150;
const int turnSpeed = 130;
const int reverseSpeed = 130;

// Buzzer timing
unsigned long lastBuzzerToggleMillis = 0;
const long buzzerOnDuration = 100;
const long buzzerOffDuration = 400;
bool buzzerIsOn = false;
bool obstacleDetectedFlag = false;
bool fireDetectedFlag = false;

// Sensor Timing
unsigned long lastLeftObstacleTime = 0;
unsigned long lastRightObstacleTime = 0;
const long obstacleDetectionThreshold = 700;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Load custom characters
  lcd.createChar(0, arrowUp);
  lcd.createChar(1, arrowDown);
  lcd.createChar(2, arrowLeft);
  lcd.createChar(3, arrowRight);
  lcd.createChar(4, fireChar);

  lcd.setCursor(0, 0);
  lcd.print("Robot Status:");

  pinMode(irSensorLeftPin, INPUT);
  pinMode(irSensorRightPin, INPUT);
  pinMode(fireSensorAnalogPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  motor1.setSpeed(motorSpeed);
  motor2.setSpeed(motorSpeed);
  motor3.setSpeed(motorSpeed);
  motor4.setSpeed(motorSpeed);

  stopMotors();
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();

  int leftSensorStatus = digitalRead(irSensorLeftPin);
  int rightSensorStatus = digitalRead(irSensorRightPin);
  int fireAnalogValue = analogRead(fireSensorAnalogPin);

  bool leftObstacle = (leftSensorStatus == LOW);
  bool rightObstacle = (rightSensorStatus == LOW);
  bool anyObstacleCurrentlyDetected = (leftObstacle || rightObstacle);

  if (leftObstacle) lastLeftObstacleTime = currentMillis;
  if (rightObstacle) lastRightObstacleTime = currentMillis;

  bool currentFireDetected = (fireAnalogValue < fireSensorAnalogThreshold);

  // --- Fire Handling ---
  if (currentFireDetected) {
    if (!fireDetectedFlag) {
      Serial.println("!!! FIRE DETECTED !!!");
      lcd.setCursor(0, 1);
      lcd.write(byte(4)); // Fire symbol
      lcd.print(" !!! FIRE !!!");
    }
    stopMotors();
    digitalWrite(buzzerPin, HIGH);
    fireDetectedFlag = true;
    return;
  } else {
    if (fireDetectedFlag) {
      digitalWrite(buzzerPin, LOW);
      Serial.println("Fire cleared. Resuming.");
      lcd.setCursor(0, 1);
      lcd.print("No Fire       ");
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.write(byte(0)); // ↑
      lcd.print(" Moving Forward");
    }
    fireDetectedFlag = false;
  }

  // --- Obstacle Handling ---
  if (anyObstacleCurrentlyDetected) {
    if (!obstacleDetectedFlag) {
      Serial.println("Obstacle Detected.");
      lcd.setCursor(0, 1);
      lcd.print("Obstacle !     ");

      digitalWrite(buzzerPin, HIGH);
      buzzerIsOn = true;
      lastBuzzerToggleMillis = currentMillis;

      stopMotors();
      delay(500);

      lcd.setCursor(0, 1);
      lcd.write(byte(1)); // ↓
      lcd.print(" Reversing");
      reverseMotors();
      delay(500);
      stopMotors();
      delay(200);

      if (leftObstacle && !rightObstacle) {
        Serial.println("Turning RIGHT");
        lcd.setCursor(0, 1);
        lcd.write(byte(3)); // →
        lcd.print(" Turning Right ");
        turnRight();
        delay(600);
        stopMotors();
      } else if (!leftObstacle && rightObstacle) {
        Serial.println("Turning LEFT");
        lcd.setCursor(0, 1);
        lcd.write(byte(2)); // ←
        lcd.print(" Turning Left  ");
        turnLeft();
        delay(600);
        stopMotors();
      } else {
        if (lastLeftObstacleTime < lastRightObstacleTime) {
          Serial.println("Both: Turning RIGHT");
          lcd.setCursor(0, 1);
          lcd.write(byte(3)); // →
          lcd.print(" Turning Right ");
          turnRight();
        } else {
          Serial.println("Both: Turning LEFT");
          lcd.setCursor(0, 1);
          lcd.write(byte(2)); // ←
          lcd.print(" Turning Left  ");
          turnLeft();
        }
        delay(600);
        stopMotors();
      }
    }

    // Buzzer pulsing
    if (buzzerIsOn && currentMillis - lastBuzzerToggleMillis >= buzzerOnDuration) {
      digitalWrite(buzzerPin, LOW);
      buzzerIsOn = false;
      lastBuzzerToggleMillis = currentMillis;
    } else if (!buzzerIsOn && currentMillis - lastBuzzerToggleMillis >= buzzerOffDuration) {
      digitalWrite(buzzerPin, HIGH);
      buzzerIsOn = true;
      lastBuzzerToggleMillis = currentMillis;
    }

    obstacleDetectedFlag = false;
  } else {
    if (obstacleDetectedFlag) {
      Serial.println("Path Clear. Moving forward.");
      lcd.setCursor(0, 1);
      lcd.print("Moving Forward ");
    }

    obstacleDetectedFlag = false;

    if (buzzerIsOn) {
      digitalWrite(buzzerPin, LOW);
      buzzerIsOn = false;
    }
    lastBuzzerToggleMillis = currentMillis;

    lcd.setCursor(0, 1);
    lcd.write(byte(0)); // ↑
    lcd.print(" Moving Forward");
    moveForward();
  }
}

// --- Motor Controls ---
void moveForward() {
  motor1.setSpeed(motorSpeed);
  motor2.setSpeed(motorSpeed);
  motor3.setSpeed(motorSpeed);
  motor4.setSpeed(motorSpeed);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void reverseMotors() {
  motor1.setSpeed(reverseSpeed);
  motor2.setSpeed(reverseSpeed);
  motor3.setSpeed(reverseSpeed);
  motor4.setSpeed(reverseSpeed);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void stopMotors() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void turnRight() {
  motor1.setSpeed(turnSpeed);
  motor2.setSpeed(turnSpeed);
  motor3.setSpeed(turnSpeed);
  motor4.setSpeed(turnSpeed);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void turnLeft() {
  motor1.setSpeed(turnSpeed);
  motor2.setSpeed(turnSpeed);
  motor3.setSpeed(turnSpeed);
  motor4.setSpeed(turnSpeed);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}
