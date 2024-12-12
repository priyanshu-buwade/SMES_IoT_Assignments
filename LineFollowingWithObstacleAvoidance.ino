#include <AFMotor.h>
#include <Servo.h>

// Motor Initialization
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// IR Sensor Pins
const int LEFT_SENSOR_PIN = A0;   // Left IR sensor
const int RIGHT_SENSOR_PIN = A1;  // Right IR sensor

// Ultrasonic Sensor Pins
const int TRIG_PIN = A4;
const int ECHO_PIN = A5;

// Servo Motor
Servo servo;
const int SERVO_PIN = 10;

// Ultrasonic sensor constants
const int OBSTACLE_DISTANCE = 15;

void setup() {
  Serial.begin(9600);

  // Initialize motors
  motor1.setSpeed(100);
  motor2.setSpeed(100);
  motor3.setSpeed(100);
  motor4.setSpeed(100);

  pinMode(LEFT_SENSOR_PIN, INPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT);

  // Initialize servo
  servo.attach(SERVO_PIN);
  servo.write(90);  // Center position

  releaseMotors();  // Ensure motors are stopped at start
}

void loop() {

  int leftSensor = digitalRead(LEFT_SENSOR_PIN);
  int rightSensor = digitalRead(RIGHT_SENSOR_PIN);

  // Check for obstacle
  if (checkObstacle()) {
    avoidObstacle();  // If obstacle detected, avoid it
    return;
  }

  // Logic for line following
  if (leftSensor == LOW && rightSensor == LOW) {
    moveForward();  // Both sensors detect the line
  } else if (leftSensor == HIGH && rightSensor == LOW) {
    turnRight();  // Left sensor off the line
  } else if (leftSensor == LOW && rightSensor == HIGH) {
    turnLeft();  // Right sensor off the line
  } else if (leftSensor == HIGH && rightSensor == HIGH) {
    releaseMotors();  // Both sensors off the line, stop
  }
}

// Method for moving motors forward
void moveForward() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

// Method for moving motors backward
void moveBackward() {
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

// Method for turning left
void turnLeft() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(RELEASE);  // Stop right motors
  motor4.run(RELEASE);
}

// Method for turning right
void turnRight() {
  motor1.run(RELEASE);  // Stop left motors
  motor2.run(RELEASE);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

// Check for obstacle using ultrasonic sensor
bool checkObstacle() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;  // Convert to cm

  if (distance > 0 && distance <= OBSTACLE_DISTANCE) {
    return true;
  }
  return false;
}

// Avoid obstacle logic
void avoidObstacle() {
  releaseMotors();
  delay(500);

  // Check left and right using servo
  int distanceLeft, distanceRight;

  // Check left
  servo.write(135);
  delay(750);
  distanceLeft = measureDistance();

  // Check right
  servo.write(45);
  delay(750);
  distanceRight = measureDistance();

  // Reset servo to center
  servo.write(90);

  if (distanceLeft > distanceRight) {
    turnLeft();
    delay(500);
    moveForward();
    delay(1000);
    turnRight();
  } else {
    turnRight();
    delay(500);
    moveForward();
    delay(1000);
    turnLeft();
  }
}

// Measure distance using ultrasonic sensor
int measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;  // Convert to cm
}

// Release all motors
void releaseMotors() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
