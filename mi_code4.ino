#include <LiquidCrystal.h>
#include <TimerOne.h>

// LCD Pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Ultrasonic Sensor Pins
const int trigPin = 9;
const int echoPin = 10;

// Heart Rate Sensor Pin
const int heartRatePin = A0;
int hbcount = 0;
int hbcheck = 0;
int timeinsec = 0;
int hbpermin = 0;
const int hbstart = 13; // Changed from 2 to 13
int hbstartcheck = 0;

// Motor Driver Pins
const int motorPin1 = 6;
const int motorPin2 = 7;

// Buzzer Pin
const int buzzerPin = 8;

void setup() {
  // Initialize LCD
  lcd.begin(20, 4);

  // Set up Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set up Motor Pins
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  // Set up Buzzer
  pinMode(buzzerPin, OUTPUT);

  // Set up Heart Rate Sensor
  pinMode(heartRatePin, INPUT);
  pinMode(hbstart, INPUT_PULLUP); // Changed from 2 to 13

  // Set up Timer1
  Timer1.initialize(800000);
  Timer1.attachInterrupt(TimerIsr);

  // Initialize LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current HR: ");
  lcd.setCursor(0, 1);
  lcd.print("Time in sec: ");
  lcd.setCursor(0, 2);
  lcd.print("HB per Min: 0.0");

  // Set up Serial Monitor
  Serial.begin(9600);
}

void loop() {
  // Heart Rate Monitoring
  if (digitalRead(hbstart) == LOW) { // Changed from 2 to 13
    hbstartcheck = 1;
    lcd.setCursor(0, 2);
    lcd.print("HB per Min: ");
    Serial.println("Start button pressed");
  }

  if (hbstartcheck == 1) {
    if ((digitalRead(heartRatePin) == HIGH) && (hbcheck == 0)) {
      hbcount++;
      hbcheck = 1;
      lcd.setCursor(14, 0);
      lcd.print(hbcount);
      lcd.print(" ");
      Serial.print("hbcount: ");
      Serial.println(hbcount);
    }

    if (digitalRead(heartRatePin) == LOW) {
      hbcheck = 0;
    }

    if (timeinsec == 10) {
      hbpermin = hbcount * 6;
      hbstartcheck = 0;
      lcd.setCursor(14, 2);
      lcd.print(hbpermin);
      lcd.print(" ");
      Serial.print("hbpermin: ");
      Serial.println(hbpermin);
      hbcount = 0;
      timeinsec = 0;
    }
  }

  // Obstacle Avoidance
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;

  if (distance <= 200) {
    // Stop motors
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    // Sound buzzer
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
    Serial.println("Obstacle detected, stopping motors and sounding buzzer");
  } else {
    // Move forward
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    Serial.println("No obstacle, moving forward");
  }

  // Display distance on LCD
  lcd.setCursor(0, 3); // Moved to the next row to avoid overwriting the heart rate information
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm");
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}

void TimerIsr() {
  if (hbstartcheck == 1) {
    timeinsec++;
    lcd.setCursor(14, 1);
    lcd.print(timeinsec);
    lcd.print(" ");
    Serial.print("timeinsec: ");
    Serial.println(timeinsec);
  }
}
