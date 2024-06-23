#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define MOTOR_IN1 5
#define MOTOR_IN2 6
#define MOTOR_IN3 7
#define MOTOR_IN4 8
#define MOTOR_ENA 9
#define MOTOR_ENB 10

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);
  pinMode(MOTOR_ENB, OUTPUT);

  setupCamera();
}

void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void moveForward() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN3, HIGH);
  digitalWrite(MOTOR_IN4, LOW);
  analogWrite(MOTOR_ENA, 200);
  analogWrite(MOTOR_ENB, 200);
}

void stopMotors() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);
}

void detectPlant() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  int greenPixelCount = 0;
  for (int i = 0; i < fb->len; i += 3) {
    int r = fb->buf[i];
    int g = fb->buf[i + 1];
    int b = fb->buf[i + 2];
    if (g > r && g > b) {
      greenPixelCount++;
    }
  }

  if (greenPixelCount > 1000) {
    stopMotors();
    takePhoto(fb);
  }

  esp_camera_fb_return(fb);
}

void takePhoto(camera_fb_t * fb) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, "http://your_server_address/upload");
  http.addHeader("Content-Type", "image/jpeg");
  int httpResponseCode = http.POST(fb->buf, fb->len);

  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
  } else {
    Serial.printf("Error code: %d\n", httpResponseCode);
  }
  http.end();
}

void loop() {
  moveForward();
  delay(1000);
  detectPlant();
  delay(1000);
}
