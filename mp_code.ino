#include <DHT.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <WiFi101.h> // Use WiFi101.h for WiFi101 Shield

// Pin Definitions
#define DHTPIN 2
#define DHTTYPE DHT22
#define TRIG_PIN 3
#define ECHO_PIN 4
#define SOIL_MOISTURE_PIN A0
#define MOTOR_IN1 5
#define MOTOR_IN2 6
#define MOTOR_IN3 7
#define MOTOR_IN4 8
#define MOTOR_ENA 9
#define MOTOR_ENB 10
#define SD_CS_PIN 4 // Changed to 4 to avoid conflict with motor pins

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize GPS module
TinyGPSPlus gps;
SoftwareSerial gpsSerial(10, 11); // RX, TX for GPS module

// Initialize WiFi module
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
WiFiClient client;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);
  pinMode(MOTOR_ENB, OUTPUT);
  
  gpsSerial.begin(9600);

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  
  // Initialize WiFi
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true); // don't continue
  }

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    delay(10000);
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Read sensors
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  
  // Read ultrasonic sensor
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;
  
  // Read GPS data
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
  double latitude = gps.location.lat();
  double longitude = gps.location.lng();
  
  // Print sensor data to serial monitor
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" C, Humidity: "); Serial.print(humidity);
  Serial.print(" %, Soil Moisture: "); Serial.print(soilMoisture);
  Serial.print(", Distance: "); Serial.print(distance);
  Serial.print(" cm, Latitude: "); Serial.print(latitude);
  Serial.print(", Longitude: "); Serial.println(longitude);
  
  // Log data to SD card
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print("Temp: "); dataFile.print(temperature);
    dataFile.print(" C, Humidity: "); dataFile.print(humidity);
    dataFile.print(" %, Soil Moisture: "); dataFile.print(soilMoisture);
    dataFile.print(", Distance: "); dataFile.print(distance);
    dataFile.print(" cm, Latitude: "); dataFile.print(latitude);
    dataFile.print(", Longitude: "); dataFile.println(longitude);
    dataFile.close();
  }
  
  // Control motors based on distance
  if (distance < 20) {
    // Stop motors
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, LOW);
  } else {
    // Move forward
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);
    analogWrite(MOTOR_ENA, 200); // Set motor speed
    analogWrite(MOTOR_ENB, 200); // Set motor speed
  }
  
  // Send data via WiFi
  if (client.connect("your_server_address", 80)) {
    client.print("GET /update?temperature=");
    client.print(temperature);
    client.print("&humidity=");
    client.print(humidity);
    client.print("&soilMoisture=");
    client.print(soilMoisture);
    client.print("&distance=");
    client.print(distance);
    client.print("&latitude=");
    client.print(latitude);
    client.print("&longitude=");
    client.print(longitude);
    client.println(" HTTP/1.1");
    client.println("Host: your_server_address");
    client.println("Connection: close");
    client.println();
    client.stop();
  } else {
    Serial.println("Connection to server failed");
  }
  
  delay(1000);
}
