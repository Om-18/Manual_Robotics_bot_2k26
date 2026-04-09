#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"

// ================= RECEIVER MAC =================
uint8_t receiverAddress[] = {0x70,0x4B,0xCA,0x8E,0xE2,0x34};

// ================= JOYSTICK 1 =================
const int VRX1_PIN = 34;
const int VRY1_PIN = 35;

// ================= JOYSTICK 2 =================
const int VRX2_PIN = 32;
const int VRY2_PIN = 33;
const int SW2_PIN  = 25;

// ================= DATA STRUCT =================
typedef struct {
  int8_t x1;
  int8_t y1;
  int8_t x2;
  int8_t y2;
  bool mode;
} Data;

Data d;

// ================= MODE =================
bool mode = 0;

// ================= LAST VALUES =================
int8_t last_x2 = 0;
int8_t last_y2 = 0;

// ================= DEBOUNCE =================
bool lastStableState = HIGH;
bool lastReading = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

// ================= STATE FUNCTIONS =================

// X 5-level
int8_t getXState(int value) {
  if (value < 1000) return -2;
  else if (value < 1500) return -1;
  else if (value < 2300) return 0;
  else if (value < 3500) return 1;
  else return 2;
}

// Y 5-level (Joystick 1)
int8_t getYState(int value) {
  if (value < 800) return -2;
  else if (value < 1600) return -1;
  else if (value < 2500) return 0;
  else if (value < 3300) return 1;
  else return 2;
}

// Y 3-level (Joystick 2)
int8_t getY3State(int value) {
  if (value < 1500) return -1;   // CLOSE
  else if (value < 2600) return 0;
  else return 1;                 // OPEN
}

// ================= BUTTON =================
void handleButton() {
  bool reading = digitalRead(SW2_PIN);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != lastStableState) {
      lastStableState = reading;

      if (lastStableState == LOW) {
        mode = !mode;
        Serial.print("Mode: ");
        Serial.println(mode == 0 ? "X" : "Y (OPEN/CLOSE)");
      }
    }
  }

  lastReading = reading;
}

// ================= SEND CALLBACK =================
void onSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sent ✅" : "Fail ❌");
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(SW2_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP NOW INIT FAILED ❌");
    return;
  }

  esp_now_register_send_cb(onSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer add failed ❌");
    return;
  }

  analogSetAttenuation(ADC_11db);

  Serial.println("🎮 Transmitter Ready");
}

// ================= LOOP =================
void loop() {

  handleButton();

  // -------- JOYSTICK 1 --------
  d.x1 = getXState(analogRead(VRX1_PIN));
  d.y1 = getYState(analogRead(VRY1_PIN));

  // -------- JOYSTICK 2 --------
  int x2_raw = analogRead(VRX2_PIN);
  int y2_raw = analogRead(VRY2_PIN);

  if (mode == 0) {
    last_x2 = getXState(x2_raw);
  } else {
    last_y2 = getY3State(y2_raw);
  }

  d.x2 = last_x2;
  d.y2 = last_y2;
  d.mode = mode;

  // -------- SEND --------
  esp_now_send(receiverAddress, (uint8_t *)&d, sizeof(d));

  // -------- DEBUG --------
  Serial.print("J1: ");
  Serial.print(d.x1); Serial.print(",");
  Serial.print(d.y1);

  Serial.print(" | J2: ");
  Serial.print(d.x2); Serial.print(",");
  Serial.print(d.y2);

  Serial.print(" | Mode: ");
  Serial.println(d.mode);

  delay(50);
}