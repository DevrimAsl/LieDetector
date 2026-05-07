#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= PINS =================
const int buttonStart = 2;   // Start Baseline
const int ledPin = 9;        // LED für Messung

const int pulsePin = A0;
const int gsrPin = A0;       // FIX: Wokwi stabil (kein echter zweiter Sensor nötig)

// ================= STATES =================
enum State {
  IDLE,          //wartet 
  BASELINE,     //normalwerte messen
  WAIT_ANSWER, //wartet auf antwort 
  MEASURE,    //misst reaktion 
  RESULT     //zeigt ergebnis 
};

State currentState = IDLE;

// ================= BUTTON =================
int lastStartState = HIGH;

// ================= WERTE =================
int baselineBPM = 0;    //ruhiger Zustand-baseline werte 
float baselineGSR = 0;  

int currentBPM = 0;     //aktuelle werte- nach frage 
float currentGSR = 0;

// ================= PULS =================
unsigned long lastBeat = 0;

// ================= FUNKTIONSDEKLARATIONEN =================
void measureBaseline();
void measureResponse();
void showResult();
int readPulse();
float readGSR();
void handleStartButton();
// ================= SETUP =================
void setup() {

  pinMode(buttonStart, INPUT_PULLUP);
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
  //prüft ständig Buttons
  //führt je nach Zustand eine Funktion aus

void loop() {

  handleStartButton();

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

    if (currentState == IDLE) {

      currentState = BASELINE;

      lcd.clear();
      lcd.print("Baseline...");
      delay(500);

    } else if (currentState == WAIT_ANSWER) {

      // egal ob JA oder NEIN → Messung starten
      currentState = MEASURE;

      lcd.clear();
      lcd.print("Messen...");

      digitalWrite(ledPin, HIGH); // LED AN
    }
  }

  lastStartState = state;
}

// ================= BASELINE =================
void measureBaseline() {

  int sumBPM = 0;
  float sumGSR = 0;

  for (int i = 0; i < 20; i++) {    //20 Messungen werden gemacht 
    sumBPM += readPulse();
    sumGSR += readGSR();
    delay(100);
  }

  baselineBPM = sumBPM / 20;    //Durchschnitt berechnet 
  baselineGSR = sumGSR / 20;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HF:");
  lcd.print(baselineBPM);

  lcd.setCursor(0,1);
  lcd.print("GSR:");
  lcd.print(baselineGSR, 0);

  delay(2000);

  lcd.clear();
  lcd.print("Baseline OK");

  delay(1500);

  currentState = WAIT_ANSWER; //Zustand 

  lcd.clear();
  lcd.print("Frage stellen");
}

// ================= RESPONSE =================
void measureResponse() {

  int sumBPM = 0;
  float sumGSR = 0;

  for (int i = 0; i < 15; i++) {    //diesmal nur 15 Werte
    sumBPM += readPulse();
    sumGSR += readGSR();
    delay(100);
  }

  currentBPM = sumBPM / 15;
  currentGSR = sumGSR / 15;

  digitalWrite(ledPin, LOW); // LED AUS

  showResult();   //Ergebnis angezeigt 
}

// ================= RESULT =================
void showResult() {

  //Ergebnis berechnet: Differenz = Veränderung
  int deltaBPM = currentBPM - baselineBPM;
  float deltaGSR = currentGSR - baselineGSR;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HF:");
  lcd.print(currentBPM);

  lcd.setCursor(0,1);
  lcd.print("d:");
  lcd.print(deltaBPM);

  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GSR:");
  lcd.print(currentGSR, 0);

  lcd.setCursor(0,1);
  lcd.print("d:");
  lcd.print(deltaGSR, 0);

  delay(2000);

  lcd.clear();

  // Bewertung
  if (deltaBPM > 10 || deltaGSR < -1000) {    //Bewertung Wenn: Puls stark steigt ODER Hautwiderstand stark sinkt
    lcd.print("Verand. HOCH");
  } else {                        //Sonst 
    lcd.print("Verand. gering");
  }

  delay(3000);

  currentState = WAIT_ANSWER;   //zurück zu nach antwort warten 

  lcd.clear();
  lcd.print("Naechste Frage");
}

// ================= SENSOR =================
int readPulse() {

  int signal = analogRead(pulsePin);

  // FIX: Simulation statt echter Pulslogik
  return map(signal, 0, 1023, 60, 120);
}

float readGSR() {

  int value = analogRead(gsrPin);

  float voltage = value * (5.0 / 1023.0);     //gsr messen 

  float resistance = (5.0 - voltage) * 10000 / voltage;

  return resistance;
}