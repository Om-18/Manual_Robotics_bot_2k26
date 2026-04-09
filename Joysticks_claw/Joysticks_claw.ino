// -------- JOYSTICK 2 PINS --------
const int VRX2_PIN = 32;
const int VRY2_PIN = 33;
const int SW2_PIN  = 25;

// -------- STATES --------
enum XState {
  BACKWARD_L2 = -2,
  BACKWARD_L1 = -1,
  CENTER_X    = 0,
  FORWARD_L1  = 1,
  FORWARD_L2  = 2
};

enum Y3State {
  CLOSE    = -1,
  CENTER_Y = 0,
  OPEN     = 1
};

// -------- MODE --------
bool mode = 0; // 0 = X mode, 1 = Y mode

// -------- DEBOUNCE VARIABLES --------
bool lastStableState = HIGH;
bool lastReading = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

// -------- X AXIS (5 LEVELS) --------
XState getXState(int value) {
  if (value < 1000) return BACKWARD_L2;
  else if (value < 1500) return BACKWARD_L1;
  else if (value < 2300) return CENTER_X;
  else if (value < 3500) return FORWARD_L1;
  else return FORWARD_L2;
}

// -------- Y AXIS (3 STATES) --------
Y3State getY3State(int value) {
  if (value < 1500) return CLOSE;
  else if (value < 2600) return CENTER_Y;
  else return OPEN;
}

// -------- BUTTON HANDLER --------
void handleButton() {
  bool reading = digitalRead(SW2_PIN);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != lastStableState) {
      lastStableState = reading;

      // Detect press (HIGH → LOW)
      if (lastStableState == LOW) {
        mode = !mode;
        Serial.print("Mode Changed to: ");
        Serial.println(mode == 0 ? "X MODE" : "Y MODE");
      }
    }
  }

  lastReading = reading;
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(SW2_PIN, INPUT_PULLUP);
  analogSetAttenuation(ADC_11db); // full ADC range

  Serial.println("System Ready...");
}

// -------- LOOP --------
void loop() {

  // Handle button press
  handleButton();

  int xValue = analogRead(VRX2_PIN);
  int yValue = analogRead(VRY2_PIN);

  // -------- MODE 0 → X CONTROL --------
  if (mode == 0) {
    XState xState = getXState(xValue);

    Serial.print("X Mode | Raw: ");
    Serial.print(xValue);
    Serial.print(" | ");

    switch (xState) {
      case BACKWARD_L2: Serial.println("BACKWARD L2"); break;
      case BACKWARD_L1: Serial.println("BACKWARD L1"); break;
      case CENTER_X:    Serial.println("CENTER"); break;
      case FORWARD_L1:  Serial.println("FORWARD L1"); break;
      case FORWARD_L2:  Serial.println("FORWARD L2"); break;
    }
  }

  // -------- MODE 1 → Y CONTROL --------
  else {
    Y3State yState = getY3State(yValue);

    Serial.print("Y Mode | Raw: ");
    Serial.print(yValue);
    Serial.print(" | ");

    switch (yState) {
      case CLOSE:    Serial.println("CLOSE"); break;
      case CENTER_Y: Serial.println("CENTER"); break;
      case OPEN:     Serial.println("OPEN"); break;
    }
  }

  delay(120);
}