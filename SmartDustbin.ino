#include <Wire.h>                // For I2C communication
#include <LiquidCrystal_I2C.h>    // Library for LCD with I2C
#include <Servo.h>                // Library for controlling the servo motor

// Initialize the LCD with I2C address 0x27, and a screen size of 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize the servo
Servo lidServo;

// Pin Definitions for Ultrasonic Sensors
const int trigPin1 = 9;  // For level detection
const int echoPin1 = 10;

const int trigPin2 = 7;  // For person detection
const int echoPin2 = 8;

// Variables for ultrasonic sensors
long duration;
int distance;
int personDistance;
int binHeight = 10;  // The height of your dustbin in cm (adjust this to your bin height)
int levelPercentage = 0;  // The fill level percentage

// Servo motor positions
int lidOpenPosition = 210;  // Adjust this for the open angle
int lidClosePosition = 90;  // Adjust this for the closed position

void setup() {
  // Set up pins for ultrasonic sensors
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // Initialize the servo motor and move it to the closed position
  lidServo.attach(6);  // Servo connected to digital pin 6
  lidServo.write(lidClosePosition);  // Ensure lid starts closed

  // Initialize the LCD
  lcd.init();
  lcd.backlight();  // Turn on the backlight of the LCD

  // Set up serial communication for debugging (optional)
  Serial.begin(9600);

  // Display welcome message on LCD
  lcd.setCursor(0, 0);
  lcd.print("Smart Dustbin");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Measure distance using the first ultrasonic sensor (bin level)
  distance = getDistance(trigPin1, echoPin1);
  
  // Calculate fill level percentage
  if (distance <= binHeight) {
    levelPercentage = map(distance, 0, binHeight, 100, 0);  // Convert distance to percentage
  } else {
    levelPercentage = 0;  // If the distance exceeds bin height, the bin is considered empty
  }

  // Display level on the LCD
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(levelPercentage);
  lcd.print("%   ");
  
  // Display bin status
  lcd.setCursor(0, 1);
  if (levelPercentage > 80) {
    lcd.print("Status: Full   ");
  } else if (levelPercentage > 40) {
    lcd.print("Status: Half   ");
  } else {
    lcd.print("Status: Empty  ");
  }

  // Measure distance using the second ultrasonic sensor (person detection)
  personDistance = getDistance(trigPin2, echoPin2);
  
  // Check if a person is near the bin (within 30 cm range)
  if (personDistance > 0 && personDistance <= 30) {
    // Open the lid
    lidServo.write(lidOpenPosition);
    Serial.println("Person detected, opening lid...");
    delay(5000);  // Keep the lid open for 5 seconds
    lidServo.write(lidClosePosition);  // Close the lid
    Serial.println("Lid closing...");
  }

  // Debug output to serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Level: ");
  Serial.print(levelPercentage);
  Serial.print("%, Person: ");
  Serial.print(personDistance);
  Serial.println(" cm");
  
  delay(1000);  // Wait 1 second before repeating the loop
}

// Function to measure distance using ultrasonic sensor
int getDistance(int trigPin, int echoPin) {
  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echo pin
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance (in cm)
  int distance = duration * 0.034 / 2;
  
  return distance;
}
