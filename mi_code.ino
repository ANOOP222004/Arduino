#include <LiquidCrystal.h>

// LCD Pins
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Ultrasonic Sensor Pins
const int trigPin = 9;
const int echoPin = 10;

// Heart Rate Sensor Pin
const int heartRatePin = A0;

// Motor Driver Pins
const int motorPin1 = 6;
const int motorPin2 = 7;

// Buzzer Pin
const int buzzerPin = 8;

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);

  // Set up Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set up Motor Pins
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  // Set up Buzzer
  pinMode(buzzerPin, OUTPUT);
  
  // Set up Serial Monitor
  Serial.begin(9600);
}

void loop() {
  // Heart Rate Monitoring
  int heartRate = analogRead(heartRatePin);
  lcd.setCursor(0, 0);
  lcd.print("HR: ");
  lcd.print(heartRate);
  
  // Obstacle Avoidance
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;

  if (distance < 20) {
    // Stop motors
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    // Sound buzzer
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
  } else {
    // Move forward
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  }

  // Display distance on LCD
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm");

  delay(500);
}
