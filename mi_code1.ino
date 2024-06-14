#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Heart Rate Sensor
const int heartRatePin = A0;
int heartRateValue = 0;

// Ultrasonic Sensor
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;

// Motors
const int enableA = 3;
const int motorA1 = 2;
const int motorA2 = 4;
const int enableB = 5;
const int motorB1 = 6;
const int motorB2 = 7;

// Buzzer
const int buzzer = 8;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust the I2C address if necessary

unsigned long previousMillis = 0;
const long interval = 100; // Adjust this value to control the update frequency

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Initialize Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize Motors
  pinMode(enableA, OUTPUT);
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(enableB, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  // Initialize Buzzer
  pinMode(buzzer, OUTPUT);

  // Start Motors
  digitalWrite(enableA, HIGH);
  digitalWrite(enableB, HIGH);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read Heart Rate
    heartRateValue = analogRead(heartRatePin);

    // Calculate Distance
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    // Display Heart Rate and Distance on LCD
    lcd.setCursor(0, 0);
    lcd.print("Heart Rate: ");
    lcd.print(heartRateValue);
    lcd.setCursor(0, 1);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print(" cm");
  }

  // Obstacle Avoidance Logic
  if (distance < 15) {
    // Stop Motors
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, LOW);

    // Activate Buzzer
    digitalWrite(buzzer, HIGH);
  } else {
    // Move Forward
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);

    // Deactivate Buzzer
    digitalWrite(buzzer, LOW);
  }
}
