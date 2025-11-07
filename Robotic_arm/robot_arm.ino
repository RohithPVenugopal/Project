#include <Servo.h>

Servo fingerServo;
const int servoPin = 8;
int currentAngle = 90;

void setup() {
  Serial.begin(115200);
  fingerServo.attach(servoPin);
  fingerServo.write(currentAngle);
  Serial.println("ARDUINO_READY");
}

String inputString = "";
bool stringComplete = false;

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      if (inputString.length() > 0) stringComplete = true;
    } else {
      inputString += inChar;
    }
  }

  if (stringComplete) {
    int angle = inputString.toInt();
    inputString = "";
    stringComplete = false;

    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    fingerServo.write(angle);
  }
}