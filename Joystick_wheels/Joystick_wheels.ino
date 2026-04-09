const int VRX_PIN = 34; // Forward/Backward
const int VRY_PIN = 35; // Left/Right

// X-axis states (same as yours)
enum XState {
  BACKWARD_L2 = -2,
  BACKWARD_L1 = -1,
  CENTER_X    = 0,
  FORWARD_L1  = 1,
  FORWARD_L2  = 2
};

// Y-axis states (NEW)
enum YState {
  LEFT_L2  = -2,
  LEFT_L1  = -1,
  CENTER_Y = 0,
  RIGHT_L1 = 1,
  RIGHT_L2 = 2
};

// -------- X AXIS --------
XState getXState(int value) {
  if (value < 1000) return BACKWARD_L2;
  else if (value < 1500) return BACKWARD_L1;
  else if (value < 2300) return CENTER_X;
  else if (value < 3500) return FORWARD_L1;
  else return FORWARD_L2;
}

// -------- Y AXIS (DIFFERENT THRESHOLDS) --------
YState getYState(int value) {
  if (value < 800) return LEFT_L2;
  else if (value < 1700) return LEFT_L1;
  else if (value < 2000) return CENTER_Y;
  else if (value < 3300) return RIGHT_L1;
  else return RIGHT_L2;
}

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  int xValue = analogRead(VRX_PIN);
  int yValue = analogRead(VRY_PIN);

  XState xState = getXState(xValue);
  YState yState = getYState(yValue);

  Serial.print("X: ");
  Serial.print(xValue);
  Serial.print(" | ");

  Serial.print("Y: ");
  Serial.print(yValue);
  Serial.print(" | ");

  // -------- PRINT X --------
  switch (xState) {
    case BACKWARD_L2: Serial.print("B2 "); break;
    case BACKWARD_L1: Serial.print("B1 "); break;
    case CENTER_X:    Serial.print("C ");  break;
    case FORWARD_L1:  Serial.print("F1 "); break;
    case FORWARD_L2:  Serial.print("F2 "); break;
  }

  // -------- PRINT Y --------
  switch (yState) {
    case LEFT_L2:  Serial.println("L2"); break;
    case LEFT_L1:  Serial.println("L1"); break;
    case CENTER_Y: Serial.println("C");  break;
    case RIGHT_L1: Serial.println("R1"); break;
    case RIGHT_L2: Serial.println("R2"); break;
  }

  delay(150);
}