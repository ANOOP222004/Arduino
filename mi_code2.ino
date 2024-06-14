// Ultrasonic Sensor
const int trigPin = 6;
const int echoPin = 7;

// Motors
const int fwdrot_lftmotor = 2;
const int bwdrot_lftmotor = 3;
const int fwdrot_rgtmotor = 4;
const int bwdrot_rgtmotor = 5;

// LCD
#include <LiquidCrystal.h>
LiquidCrystal lcd(13,12,8,10,11);
long duration;
int distance;

void setup() {
  // Initialize Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize Motors
  pinMode(fwdrot_lftmotor, OUTPUT);
  pinMode(bwdrot_lftmotor, OUTPUT);
  pinMode(fwdrot_rgtmotor, OUTPUT);
  pinMode(bwdrot_rgtmotor, OUTPUT);
  lcd.begin(16,2);

void loop() {
    // Calculate Distance
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    // Display Heart Rate and Distance on LCD
    lcd.setCursor(0,0);
    lcd.print(distance);
    lcd.setCursor(9,0);
    lcd.print("Distance: ");
    lcd.print(" cm");
  }

  // Obstacle Avoidance Logic
  if (distance >= 500) {
    // rotate in opposite direction
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print('moving back');
    lcd.setCursor(0,0);
    lcd.print(distance);
    lcd.setCursor(9,0);
    lcd.print("Distance: ");
    lcd.print(" cm");
    digitalWrite(bwdrot_lftmotor, HIGH);
    digitalWrite(bwdrot_rgtmotor, HIGH);
    digitalWrite(fwdrot_lftmotor, LOW);
    digitalWrite(fwdrot_rgtmotor, LOW);
    delay(1000);

    //moving left/right
     lcd.clear();
    lcd.setCursor(0,1);
    lcd.print('moving left');
    lcd.setCursor(0,0);
    lcd.print(distance);
    lcd.setCursor(9,0);
    lcd.print("Distance: ");
    lcd.print(" cm");
    digitalWrite(bwdrot_lftmotor, HIGH);
    digitalWrite(bwdrot_rgtmotor, LOW);
    digitalWrite(fwdrot_lftmotor, LOW);
    digitalWrite(fwdrot_rgtmotor, HIGH);
    delay(500);
  } 
  else {
    // Move Forward
     lcd.clear();
    lcd.setCursor(0,1);
    lcd.print('moving forward');
    lcd.setCursor(0,0);
    lcd.print(distance);
    lcd.setCursor(9,0);
    lcd.print("Distance: ");
    lcd.print(" cm");
    digitalWrite(bwdrot_lftmotor, LOW);
    digitalWrite(bwdrot_rgtmotor, LOW);
    digitalWrite(fwdrot_lftmotor, HIGH);
    digitalWrite(fwdrot_rgtmotor, HIGH);
    delay(1000);
  }
}
