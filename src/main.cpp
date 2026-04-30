#include <Arduino.h>            // Grundfunktionen des Arduino
#include <Wire.h>              // Für I2C Kommunikation
#include <LiquidCrystal_I2C.h> // LCD über I2C ansteuern

// LCD initialisieren: Adresse 0x27, 16 Zeichen, 2 Zeilen
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= PINS =================
const int buttonPin = 2; // Button zum Starten
const int pulsePin = A0; // Pulssensor (Herzfrequenz)
const int gsrPin = A1;   // GSR Sensor (Hautwiderstand)

// ================= ZUSTÄNDE =================
// Zustandsmaschine für Programmablauf
enum State {
  IDLE,              // Wartet auf Start
  BASELINE,          // Grundwerte messen
  WAIT_QUESTION,     // Warten auf Frage
  WAIT_ANSWER,       // Warten auf Antwort
  MEASURE_RESPONSE,  // Reaktion messen
  RESULT             // Ergebnis anzeigen
};

// aktueller Zustand
State currentState = IDLE;

// ================= BUTTON =================
int lastButtonState = HIGH; // letzter Zustand (für Flankenerkennung)

// ================= WERTE =================
int baselineBPM = 0;   // Ruhe-Herzfrequenz
float baselineGSR = 0; // Ruhe-GSR

int currentBPM = 0;    // aktuelle Herzfrequenz
float currentGSR = 0;  // aktueller GSR

// ================= PULS =================
unsigned long lastBeat = 0; // Zeitpunkt des letzten Herzschlags

// ================= SERIAL =================
String inputString = ""; // Eingaben vom PC

// ================= FUNKTIONSDEKLARATION =================
void showIdle();
void handleButton();
void readSerial();
void measureBaseline();
void measureResponse();
void showResult();
int readPulse();
float readGSR();

// ================= SETUP =================
void setup() {

  // Button als Input mit internem Pullup-Widerstand
  pinMode(buttonPin, INPUT_PULLUP);

  // LCD starten
  lcd.init();
  lcd.backlight();

  // Serielle Verbindung starten
  Serial.begin(9600);

  // Startanzeige
  lcd.print("Luegendetektor");
  delay(2000);

  lcd.clear();

  // Idle Anzeige anzeigen
  showIdle();
}

// ================= LOOP =================
void loop() {

  // Prüfen ob Button gedrückt wurde
  handleButton();

  // Prüfen ob Daten vom PC kommen
  readSerial();

  // Zustandsmaschine
  switch(currentState) {

    case IDLE:
      // nichts tun
      break;

    case BASELINE:
      // Grundwerte messen
      measureBaseline();
      break;

    case WAIT_QUESTION:
      // warten auf FRAGE vom PC
      break;

    case WAIT_ANSWER:
      // warten auf ANTWORT vom PC
      break;

    case MEASURE_RESPONSE:
      // Reaktion messen
      measureResponse();
      break;

    case RESULT:
      // wird aktuell nicht separat genutzt
      break;
  }
}

// ================= BUTTON =================
void handleButton() {

  int buttonState = digitalRead(buttonPin); // aktuellen Zustand lesen

  // Wenn Button gedrückt wurde (HIGH → LOW)
  if (lastButtonState == HIGH && buttonState == LOW) {

    currentState = BASELINE; // starte Baseline Messung

    lcd.clear();
    lcd.print("Baseline...");
    delay(500);
  }

  // aktuellen Zustand speichern
  lastButtonState = buttonState;
}

// ================= SERIAL =================
void readSerial() {

  // prüfen ob Daten vorhanden sind
  if (Serial.available()) {

    // lese bis Zeilenende
    inputString = Serial.readStringUntil('\n');

    // Wenn "FRAGE" empfangen wird
    if (inputString.startsWith("FRAGE")) {

      lcd.clear();
      lcd.print("Frage gestartet");

      currentState = WAIT_ANSWER;
    }

    // Wenn "ANTWORT" empfangen wird
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

  // 20 Messungen durchführen
  for (int i = 0; i < 20; i++) {

    sumBPM += readPulse(); // Puls lesen
    sumGSR += readGSR();   // GSR lesen

    delay(200); // kurze Pause
  }

  // Durchschnitt berechnen
  baselineBPM = sumBPM / 20;
  baselineGSR = sumGSR / 20;

  // Anzeige
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Baseline OK");

  lcd.setCursor(0,1);
  lcd.print("HF:");
  lcd.print(baselineBPM);

  delay(2000);

  // Weiter zum nächsten Zustand
  currentState = WAIT_QUESTION;

  lcd.clear();
  lcd.print("Warte Frage...");
}

// ================= RESPONSE =================
void measureResponse() {

  int sumBPM = 0;
  float sumGSR = 0;

  // 15 Messungen durchführen
  for (int i = 0; i < 15; i++) {

    sumBPM += readPulse();
    sumGSR += readGSR();

    delay(200);
  }

  // Durchschnitt berechnen
  currentBPM = sumBPM / 15;
  currentGSR = sumGSR / 15;

  // Ergebnis anzeigen
  showResult();
}

// ================= RESULT =================
void showResult() {

  // Differenz berechnen (Veränderung)
  int deltaBPM = currentBPM - baselineBPM;
  float deltaGSR = currentGSR - baselineGSR;

  // Herzfrequenz anzeigen
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HF:");
  lcd.print(currentBPM);

  lcd.setCursor(0,1);
  lcd.print("d:");
  lcd.print(deltaBPM);

  delay(3000);

  // GSR anzeigen
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GSR:");
  lcd.print(currentGSR, 0);

  lcd.setCursor(0,1);
  lcd.print("d:");
  lcd.print(deltaGSR, 0);

  delay(3000);

  lcd.clear();

  // Einfache Stressbewertung
  if (deltaBPM > 10 || deltaGSR < -1000) {

    lcd.print("Stress HOCH!");

  } else {

    lcd.print("Stress niedrig");
  }

  delay(4000);

  // Zurück zum Fragen-Modus
  currentState = WAIT_QUESTION;

  lcd.clear();
  lcd.print("Naechste Frage");
}

// ================= SENSOR =================

// Puls messen
int readPulse() {

  int signal = analogRead(pulsePin); // Signal lesen

  // Schwellenwert für Herzschlag
  if (signal > 600) {

    unsigned long now = millis();

    // Mindestabstand zwischen Schlägen
    if (now - lastBeat > 300) {

      // BPM berechnen
      int bpm = 60000 / (now - lastBeat);

      lastBeat = now;

      return bpm;
    }
  }

  return 0; // kein gültiger Wert
}

// GSR messen
float readGSR() {

  int value = analogRead(gsrPin); // Rohwert

  // Spannung berechnen
  float voltage = value * (5.0 / 1023.0);

  // Widerstand berechnen (Formel)
  float resistance = (5.0 - voltage) * 10000 / voltage;

  return resistance;
}

// ================= DISPLAY =================
void showIdle() {

  lcd.setCursor(0,0);
  lcd.print("Taste starten");
}