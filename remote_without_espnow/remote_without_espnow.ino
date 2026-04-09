// ================= JOYSTICK 1 =================
const int VRX1_PIN = 34;
const int VRY1_PIN = 35;

// ================= JOYSTICK 2 =================
const int VRX2_PIN = 32;
const int VRY2_PIN = 33;
const int SW2_PIN  = 25;

// ================= STATES =================
enum XState {
  BACKWARD_L2 = -2,
  BACKWARD_L1 = -1,
  CENTER_X    = 0,
  FORWARD_L1  = 1,
  FORWARD_L2  = 2
};

enum YState {
  LEFT_L2  = -2,
  LEFT_L1  = -1,
  CENTER_Y = 0,
  RIGHT_L1 = 1,
  RIGHT_L2 = 2
};

enum Y3State {
  CLOSE    = -1,
  CENTER_Y2 = 0,
  OPEN     = 1
};

// ================= MODE =================
bool mode = 0; // 0 = X, 1 = Y (open/close)

// ================= DEBOUNCE =================
bool lastStableState = HIGH;
bool lastReading = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

// ================= FUNCTIONS =================

// Joystick 1 X (F/B)
XState getX1State(int value) {
  if (value < 1000) return BACKWARD_L2;
  else if (value < 1500) return BACKWARD_L1;
  else if (value < 2300) return CENTER_X;
  else if (value < 3500) return FORWARD_L1;
  else return FORWARD_L2;
}

// Joystick 1 Y (L/R)
YState getY1State(int value) {
  if (value < 800) return LEFT_L2;
  else if (value < 1600) return LEFT_L1;
  else if (value < 2500) return CENTER_Y;
  else if (value < 3300) return RIGHT_L1;
  else return RIGHT_L2;
}

// Joystick 2 X (same 5 levels)
XState getX2State(int value) {
  if (value < 1000) return BACKWARD_L2;
  else if (value < 1500) return BACKWARD_L1;
  else if (value < 2300) return CENTER_X;
  else if (value < 3500) return FORWARD_L1;
  else return FORWARD_L2;
}

// Joystick 2 Y (OPEN/CLOSE)
Y3State getY2State(int value) {
  if (value < 1500) return CLOSE;
  else if (value < 2600) return CENTER_Y2;
  else return OPEN;
}

// Button handler
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
        Serial.print("Mode Changed → ");
        Serial.println(mode == 0 ? "X MODE" : "Y MODE (OPEN/CLOSE)");
      }
    }
  }

  lastReading = reading;
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(SW2_PIN, INPUT_PULLUP);
  analogSetAttenuation(ADC_11db);

  Serial.println("System Ready...");
}

// ================= LOOP =================
void loop() {

  handleButton();

  // -------- JOYSTICK 1 --------
  int x1 = analogRead(VRX1_PIN);
  int y1 = analogRead(VRY1_PIN);

  XState x1State = getX1State(x1);
  YState y1State = getY1State(y1);

  // -------- JOYSTICK 2 --------
  int x2 = analogRead(VRX2_PIN);
  int y2 = analogRead(VRY2_PIN);

  Serial.print("J1: ");

  // X1 Output
  switch (x1State) {
    case BACKWARD_L2: Serial.print("B2 "); break;
    case BACKWARD_L1: Serial.print("B1 "); break;
    case CENTER_X:    Serial.print("C ");  break;
    case FORWARD_L1:  Serial.print("F1 "); break;
    case FORWARD_L2:  Serial.print("F2 "); break;
  }

  // Y1 Output
  switch (y1State) {
    case LEFT_L2:  Serial.print("L2 "); break;
    case LEFT_L1:  Serial.print("L1 "); break;
    case CENTER_Y: Serial.print("C ");  break;
    case RIGHT_L1: Serial.print("R1 "); break;
    case RIGHT_L2: Serial.print("R2 "); break;
  }

  Serial.print(" | J2: ");

  // -------- JOYSTICK 2 MODE --------
  if (mode == 0) {
    XState x2State = getX2State(x2);

    switch (x2State) {
      case BACKWARD_L2: Serial.println("B2"); break;
      case BACKWARD_L1: Serial.println("B1"); break;
      case CENTER_X:    Serial.println("C");  break;
      case FORWARD_L1:  Serial.println("F1"); break;
      case FORWARD_L2:  Serial.println("F2"); break;
    }

  } else {
    Y3State y2State = getY2State(y2);

    switch (y2State) {
      case CLOSE:    Serial.println("CLOSE"); break;
      case CENTER_Y2:Serial.println("CENTER"); break;
      case OPEN:     Serial.println("OPEN"); break;
    }
  }

  delay(120);
}