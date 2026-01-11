#include <Servo.h>

// =======================
// LED INDICATRICES
// =======================
const int LED_R = A0;
const int LED_Y = A1;
const int LED_B = A2;

// =======================
// SERVOS
// =======================
Servo baseServo, armServo, elbowServo, clawLeft, clawRight;

const byte BASE_PIN  = 9;
const byte ARM_PIN   = 10;
const byte ELBOW_PIN = 11;
const byte CLAW_L_PIN = 12;
const byte CLAW_R_PIN = 13;

// Positions initiales
const int BASE_INIT  = 50;
const int BASE_DROP  = 90;
const int ARM_INIT   = 25;
const int ARM_DROP   = 0;
const int ELBOW_INIT = 70;
const int ELBOW_DROP = 120;

// =======================
// CAPTEUR COULEUR TCS
// =======================
const byte PIN_S0 = 4;
const byte PIN_S1 = 5;
const byte PIN_S2 = 6;
const byte PIN_S3 = 7;
const byte PIN_OUT = 8;

int redVal, greenVal, blueVal;
bool objectDetected = false;

// =======================
// SETUP
// =======================
void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_B, OUTPUT);

  baseServo.attach(BASE_PIN);
  armServo.attach(ARM_PIN);
  elbowServo.attach(ELBOW_PIN);
  clawLeft.attach(CLAW_L_PIN);
  clawRight.attach(CLAW_R_PIN);

  initPosition();

  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S3, OUTPUT);
  pinMode(PIN_OUT, INPUT);

  digitalWrite(PIN_S0, HIGH);
  digitalWrite(PIN_S1, LOW);

  Serial.begin(9600);
}

// =======================
// LOOP
// =======================
void loop() {
  readColorValues();

  if (!objectDetected) {
    detectColor();
  } else {
    moveObject();
  }
}

// =======================
// LECTURE DES COULEURS
// =======================
void readColorValues() {
  digitalWrite(PIN_S2, LOW);
  digitalWrite(PIN_S3, LOW);
  redVal = pulseIn(PIN_OUT, LOW);

  digitalWrite(PIN_S2, HIGH);
  digitalWrite(PIN_S3, HIGH);
  greenVal = pulseIn(PIN_OUT, LOW);

  digitalWrite(PIN_S2, LOW);
  digitalWrite(PIN_S3, HIGH);
  blueVal = pulseIn(PIN_OUT, LOW);

  Serial.print("R:");
  Serial.print(redVal);
  Serial.print(" G:");
  Serial.print(greenVal);
  Serial.print(" B:");
  Serial.println(blueVal);

  delay(100);
}

// =======================
// DETECTION COULEUR
// =======================
void detectColor() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_B, LOW);

  if (isRed()) {
    digitalWrite(LED_R, HIGH);
    objectDetected = true;
  } 
  else if (isYellow()) {
    digitalWrite(LED_Y, HIGH);
    objectDetected = true;
  } 
  else if (isBlue()) {
    digitalWrite(LED_B, HIGH);
    objectDetected = true;
  }
}

bool isRed() {
  return (redVal >= 25 && redVal <= 45 &&
          greenVal >= 85 && greenVal <= 105 &&
          blueVal >= 55 && blueVal <= 75);
}

bool isYellow() {
  return (redVal >= 25 && redVal <= 45 &&
          greenVal >= 40 && greenVal <= 60 &&
          blueVal >= 65 && blueVal <= 85);
}

bool isBlue() {
  return (redVal >= 70 && redVal <= 90 &&
          greenVal >= 75 && greenVal <= 95 &&
          blueVal >= 40 && blueVal <= 60);
}

// =======================
// MOUVEMENT DU BRAS
// =======================
void initPosition() {
  baseServo.write(BASE_INIT);
  armServo.write(ARM_INIT);
  elbowServo.write(ELBOW_INIT);
  clawLeft.write(90);
  clawRight.write(90);
}

void moveObject() {
  closeClaw();

  smoothMove(armServo, ARM_INIT, ARM_DROP);
  smoothMove(elbowServo, ELBOW_INIT, ELBOW_DROP);
  smoothMove(baseServo, BASE_INIT, BASE_DROP);

  openClaw();
  delay(2000);

  smoothMove(baseServo, BASE_DROP, BASE_INIT);
  smoothMove(elbowServo, ELBOW_DROP, ELBOW_INIT);
  smoothMove(armServo, ARM_DROP, ARM_INIT);

  initPosition();
  objectDetected = false;
}

void smoothMove(Servo &motor, int fromPos, int toPos) {
  int step = (fromPos < toPos) ? 1 : -1;
  for (int pos = fromPos; pos != toPos; pos += step) {
    motor.write(pos);
    delay(30);
  }
}

// =======================
// PINCE
// =======================
void openClaw() {
  clawLeft.write(70);
  clawRight.write(100);
}

void closeClaw() {
  clawLeft.write(100);
  clawRight.write(80);
}

