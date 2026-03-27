#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


// Pins
const int buttonPin = 2;
const int pulsePin = A0;
const int gsrPin = A1;

// Zustände
enum State {
  IDLE,
  BASELINE,
  WAIT_QUESTION,
  WAIT_ANSWER,
  MEASURE_RESPONSE,
  RESULT
};

State currentState = IDLE;

// Button
int lastButtonState = HIGH;

// Werte
int baselineBPM = 0;
float baselineGSR = 0;

int currentBPM = 0;
float currentGSR = 0;

// Puls
unsigned long lastBeat = 0;

// Serial
String inputString = "";

void showIdle();
void handleButton();
void readSerial();
void measureBaseline();
void measureResponse();
void showResult();
int readPulse();
float readGSR();

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);

  lcd.print("Luegendetektor");
  delay(2000);
  lcd.clear();
  showIdle();
}

void loop() {
  handleButton();
  readSerial();

  switch(currentState) {
    case IDLE:
      break;

    case BASELINE:
      measureBaseline();
      break;

    case WAIT_QUESTION:
      break;

    case WAIT_ANSWER:
      break;

    case MEASURE_RESPONSE:
      measureResponse();
      break;

    case RESULT:
      break;
  }
}

// ================= BUTTON =================
void handleButton() {
  int buttonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && buttonState == LOW) {
    currentState = BASELINE;
    lcd.clear();
    lcd.print("Baseline...");
    delay(500);
  }

  lastButtonState = buttonState;
}

// ================= SERIAL =================
void readSerial() {
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\n');

    if (inputString.startsWith("FRAGE")) {
      lcd.clear();
      lcd.print("Frage gestartet");
      currentState = WAIT_ANSWER;
    }

    if (inputString.startsWith("ANTWORT")) {
      lcd.clear();
      lcd.print("Antwort...");
      delay(1000);
      currentState = MEASURE_RESPONSE;
    }
  }
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
  lcd.print("Baseline OK");

  lcd.setCursor(0,1);
  lcd.print("HF:");
  lcd.print(baselineBPM);

  delay(2000);

  currentState = WAIT_QUESTION;
  lcd.clear();
  lcd.print("Warte Frage...");
}

// ================= RESPONSE ================
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

  // einfache Bewertung
  if (deltaBPM > 10 || deltaGSR < -1000) {
    lcd.print("Stress HOCH!");
  } else {
    lcd.print("Stress niedrig");
  }

  delay(4000);

  currentState = WAIT_QUESTION;
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

// ================= DISPLAY =================
void showIdle() {
  lcd.setCursor(0,0);
  lcd.print("Taste starten");
}