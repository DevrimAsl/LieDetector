#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= PINS =================
const int buttonStart = 2;   // Start Baseline
const int buttonAnswer = 3;  // Antwort Taste (Ja/Nein)
const int ledPin = 4;        // LED für Messung

const int pulsePin = A0;
const int gsrPin = A1;

// ================= STATES =================
enum State {
  IDLE,
  BASELINE,
  WAIT_ANSWER,
  MEASURE,
  RESULT
};

State currentState = IDLE;

// ================= BUTTON =================
int lastStartState = HIGH;
int lastAnswerState = HIGH;

// ================= WERTE =================
int baselineBPM = 0;
float baselineGSR = 0;

int currentBPM = 0;
float currentGSR = 0;

// ================= PULS =================
unsigned long lastBeat = 0;

// ================= SETUP =================
void setup() {

  pinMode(buttonStart, INPUT_PULLUP);
  pinMode(buttonAnswer, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  lcd.init();
  lcd.backlight();

  Serial.begin(9600);

  lcd.print("Luegendetektor");
  delay(2000);
  lcd.clear();

  lcd.print("Start druecken");
}

// ================= LOOP =================
void loop() {

  handleStartButton();
  handleAnswerButton();

  switch(currentState) {

    case IDLE:
      break;

    case BASELINE:
      measureBaseline();
      break;

    case WAIT_ANSWER:
      // wartet auf Antwort
      break;

    case MEASURE:
      measureResponse();
      break;

    case RESULT:
      // wird in Funktion erledigt
      break;
  }
}

// ================= START BUTTON =================
void handleStartButton() {

  int state = digitalRead(buttonStart);

  if (lastStartState == HIGH && state == LOW) {

    currentState = BASELINE;

    lcd.clear();
    lcd.print("Baseline...");
    delay(500);
  }

  lastStartState = state;
}

// ================= ANSWER BUTTON =================
void handleAnswerButton() {

  int state = digitalRead(buttonAnswer);

  if (currentState == WAIT_ANSWER) {

    if (lastAnswerState == HIGH && state == LOW) {

      // egal ob JA oder NEIN → Messung starten
      currentState = MEASURE;

      lcd.clear();
      lcd.print("Messen...");

      digitalWrite(ledPin, HIGH); // LED AN
    }
  }

  lastAnswerState = state;
}

// ================= BASELINE =================
void measureBaseline() {

  int sumBPM = 0;
  float sumGSR = 0;

  for (int i = 0; i < 20; i++) {
    sumBPM += readPulse();
    sumGSR += readGSR();
    delay(200);
  }

  baselineBPM = sumBPM / 20;
  baselineGSR = sumGSR / 20;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HF:");
  lcd.print(baselineBPM);

  lcd.setCursor(0,1);
  lcd.print("GSR:");
  lcd.print(baselineGSR, 0);

  delay(3000);

  lcd.clear();
  lcd.print("Baseline OK");

  delay(2000);

  currentState = WAIT_ANSWER;

  lcd.clear();
  lcd.print("Frage stellen");
}

// ================= RESPONSE =================
void measureResponse() {

  int sumBPM = 0;
  float sumGSR = 0;

  for (int i = 0; i < 15; i++) {
    sumBPM += readPulse();
    sumGSR += readGSR();
    delay(200);
  }

  currentBPM = sumBPM / 15;
  currentGSR = sumGSR / 15;

  digitalWrite(ledPin, LOW); // LED AUS

  showResult();
}

// ================= RESULT =================
void showResult() {

  int deltaBPM = currentBPM - baselineBPM;
  float deltaGSR = currentGSR - baselineGSR;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HF:");
  lcd.print(currentBPM);

  lcd.setCursor(0,1);
  lcd.print("d:");
  lcd.print(deltaBPM);

  delay(3000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GSR:");
  lcd.print(currentGSR, 0);

  lcd.setCursor(0,1);
  lcd.print("d:");
  lcd.print(deltaGSR, 0);

  delay(3000);

  lcd.clear();

  // Bewertung
  if (deltaBPM > 10 || deltaGSR < -1000) {
    lcd.print("Verand. HOCH");
  } else {
    lcd.print("Verand. gering");
  }

  delay(4000);

  currentState = WAIT_ANSWER;

  lcd.clear();
  lcd.print("Naechste Frage");
}

// ================= SENSOR =================
int readPulse() {

  int signal = analogRead(pulsePin);

  if (signal > 600) {

    unsigned long now = millis();

    if (now - lastBeat > 300) {

      int bpm = 60000 / (now - lastBeat);
      lastBeat = now;

      return bpm;
    }
  }

  return 0;
}

float readGSR() {

  int value = analogRead(gsrPin);

  float voltage = value * (5.0 / 1023.0);

  float resistance = (5.0 - voltage) * 10000 / voltage;

  return resistance;
}