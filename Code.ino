const int irSensorPin = A3;    // Pin connected to the IR sensor
const int ledPinG = 3;      // Pin connected to the buzzer
const int ledPinR = 4;         // Pin connected to the LED (optional)
#include <NewPing.h>

#define TRIGGER_PIN 5
#define ECHO_PIN 6
#define MAX_DISTANCE 200 // Adjust as needed

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  pinMode(irSensorPin, INPUT);  // IR sensor as input
  pinMode(ledPinG, OUTPUT);   // Buzzer as output
  pinMode(ledPinR, OUTPUT);      // LED as output (optional)
  
  Serial.begin(9600);           // For debugging
}

void loop() {
  int irValue = analogRead(irSensorPin);  // Read the IR sensor value
  Serial.println(irValue);
  int distance = sonar.ping_cm();
  if (distance > 0 && distance < MAX_DISTANCE) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("No object detected");
  }
  
  if (irValue <=500) {  // Object detected (depends on the IR sensor logic)
    digitalWrite(ledPinR, HIGH);     // Turn on LED
  } else {     
    digitalWrite(ledPinR, LOW);
  }
  if (distance <=5) {  // Object detected (depends on the IR sensor logic)
    digitalWrite(ledPinG, HIGH);     // Turn on LED
  } else {   
    digitalWrite(ledPinG, LOW);
  }
  

  delay(100);  // Small delay for stability
}
