# Obstacle Avoiding Robot Using IR Sensors (4 Wheels)

<p align="center">
  <img src="Obstacle Avoiding Robot.jpg" alt="Obstacle Avoiding Robot Front View" width="600" />
</p>

<p align="center">
  <img src="QAR Side View.jpg" alt="Obstacle Avoiding Robot Side View" width="600" />
</p>

This project is an Arduino-controlled autonomous robot that moves forward, detects obstacles using IR sensors, avoids them by reversing and turning, and also stops when a flame/fire source is detected.

The robot uses:
- 4-wheel drive for navigation
- 2 IR obstacle sensors
- 1 analog flame/fire sensor
- 1 buzzer for alerts
- 16x2 I2C LCD for status display
- Adafruit Motor Shield to control the DC motors

## Project Files

- [Obstacle_Avoiding_Robot_Using_IR_Sensors_4_Wheels_22_July_2025.ino](Obstacle_Avoiding_Robot_Using_IR_Sensors_4_Wheels_22_July_2025.ino) — main Arduino sketch

## Features

- Autonomous forward movement
- Obstacle detection with left and right IR sensors
- Reverse and steer away from obstacles
- Fire detection using an analog flame sensor
- Immediate emergency stop when fire is detected
- Audio buzzer alert during obstacle and fire events
- LCD status messages and directional icons
- Serial output for debugging via the Arduino Serial Monitor

## Hardware Components

- Arduino board
- 4 DC geared motors
- Motor driver shield (Adafruit AFMotor / Motor Shield)
- Left IR obstacle sensor
- Right IR obstacle sensor
- Flame/fire sensor module (analog output)
- 16x2 LCD with I2C interface
- Buzzer
- Battery pack / power supply for motors and controller
- Robot chassis with 4 wheels

## Pin Mapping

| Component | Arduino Pin / Interface | Notes |
| --- | --- | --- |
| Left IR sensor | A1 | Digital read, LOW = obstacle detected |
| Right IR sensor | A2 | Digital read, LOW = obstacle detected |
| Fire sensor | A0 | Analog value compared against threshold |
| Buzzer | A3 | Alert output |
| LCD | I2C address 0x27 | 16x2 display |
| Motor 1 | AFMotor 1 | Front/left wheel set |
| Motor 2 | AFMotor 2 | Front/right wheel set |
| Motor 3 | AFMotor 3 | Rear/left wheel set |
| Motor 4 | AFMotor 4 | Rear/right wheel set |

## Operating Logic

### 1. Normal movement
- The robot moves forward when no obstacle is detected.
- The LCD shows a forward arrow and the text “Moving Forward”.

### 2. Obstacle avoidance
- If the left or right IR sensor detects an obstacle, the robot:
  - stops
  - reverses briefly
  - checks which side was blocked
  - turns left or right to avoid the obstacle
- The buzzer pulses to indicate obstacle detection.

### 3. Fire detection
- The analog flame sensor value is read continuously.
- If it drops below the threshold (`400`), the robot considers fire detected.
- In that case:
  - motors stop immediately
  - buzzer turns on continuously
  - LCD shows a fire warning
  - robot remains halted until the fire is gone

## Code Behavior Summary

The sketch is structured as follows:

- `setup()` initializes serial communication, LCD, custom characters, sensors, buzzer, and motors.
- `loop()` continuously reads sensor values and decides whether the robot should move, reverse, turn, or stop.
- `moveForward()`, `reverseMotors()`, `turnLeft()`, and `turnRight()` control the four motors.
- `stopMotors()` releases motor power.

## Library Requirements

Install these Arduino libraries before uploading:

- `Adafruit Motor Shield Library` (AFMotor)
- `LiquidCrystal_I2C` library

## Upload Instructions

1. Open the `.ino` file in the Arduino IDE.
2. Connect the Arduino board to your computer.
3. Install the required libraries.
4. Select the correct board and COM port.
5. Upload the sketch.
6. Power the robot and place it on a flat surface.

## Notes

- The IR sensors are treated as active-low: a detected obstacle gives a LOW signal.
- The fire sensor threshold can be adjusted by changing `fireSensorAnalogThreshold`.
- The robot’s movement timing can be adjusted by changing:
  - `motorSpeed`
  - `turnSpeed`
  - `reverseSpeed`
  - obstacle turning delays

## Potential Improvements

- Add ultrasonic sensor for better obstacle detection
- Add line-following capability
- Add Bluetooth or Wi-Fi control
- Add automatic path memory or smart maze navigation
- Improve fire detection logic with multiple sensor inputs

## License

This project is intended for educational and hobby use.
