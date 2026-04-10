#include <esp_now.h>
#include <WiFi.h>

// ================= MOTOR PINS =================
const int LEFT_IN1  = 26;
const int LEFT_IN2  = 27;
const int RIGHT_IN3 = 14;
const int RIGHT_IN4 = 12;

const int ENA = 25;
const int ENB = 33;

// ================= PWM =================
const int freq = 5000;
const int resolution = 8;

int baseSpeed = 200;

// ================= DATA STRUCT =================
typedef struct {
  int8_t x1;
  int8_t y1;
  int8_t x2;
  int8_t y2;
  bool mode;
} Data;

Data d;

// ================= MOTOR FUNCTIONS =================
void setSpeed(int spd) {
  spd = constrain(spd, 0, 255);
  ledcWrite(ENA, spd);
  ledcWrite(ENB, spd);
}

void stopMotors() {
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN3, LOW);
  digitalWrite(RIGHT_IN4, LOW);
}

void moveForward(int spd) {
  setSpeed(spd);
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN3, HIGH);
  digitalWrite(RIGHT_IN4, LOW);
}

void moveBackward(int spd) {
  setSpeed(spd);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  digitalWrite(RIGHT_IN3, LOW);
  digitalWrite(RIGHT_IN4, HIGH);
}

void turnLeft() {
  setSpeed(150);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN3, HIGH);
  digitalWrite(RIGHT_IN4, LOW);
}

void turnRight() {
  setSpeed(150);
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN3, LOW);
  digitalWrite(RIGHT_IN4, LOW);
}

// 🔥 Tank Turns
void tankTurnLeft() {
  setSpeed(140);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  digitalWrite(RIGHT_IN3, HIGH);
  digitalWrite(RIGHT_IN4, LOW);
}

void tankTurnRight() {
  setSpeed(140);
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN3, LOW);
  digitalWrite(RIGHT_IN4, HIGH);
}

// ================= RECEIVE CALLBACK =================
void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&d, incomingData, sizeof(d));
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_IN3, OUTPUT);
  pinMode(RIGHT_IN4, OUTPUT);

  ledcAttach(ENA, freq, resolution);
  ledcAttach(ENB, freq, resolution);

  stopMotors();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP NOW INIT FAILED");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  Serial.println("🚗 Receiver Ready");
}

// ================= LOOP =================
void loop() {

  // 🔥 PRIORITY: Tank Turn (Joystick 2)
  if (d.x2 <= -1) {
    Serial.println("Tank Left");
    tankTurnLeft();
  }
  else if (d.x2 >= 1) {
    Serial.println("Tank Right");
    tankTurnRight();
  }

  // 🔹 Normal Movement (Joystick 1)
  else {

    // Forward / Backward
    if (d.x1 >= 1) {
      Serial.println("Forward");
      moveForward(map(d.x1, 1, 2, 150, 255));
    }
    else if (d.x1 <= -1) {
      Serial.println("Backward");
      moveBackward(map(abs(d.x1), 1, 2, 150, 255));
    }

    // Turning
    else if (d.y1 <= -1) {
      Serial.println("Left");
      turnLeft();
    }
    else if (d.y1 >= 1) {
      Serial.println("Right");
      turnRight();
    }

    else {
      Serial.println("Stop");
      stopMotors();
    }
  }

  delay(50);
}