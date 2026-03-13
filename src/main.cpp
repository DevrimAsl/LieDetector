#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD Adresse 0x27, 16x2 Display

const int gsrPin = A0;
const int pulsePin = A1;
const int buttonPin = 2;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.setCursor(0,0);
  lcd.print("Bereit...");
}

void loop() {
  if (digitalRead(buttonPin) == LOW) { // Button gedrückt
    int gsr = analogRead(gsrPin);
    int pulse = analogRead(pulsePin);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("GSR: "); lcd.print(gsr);
    lcd.setCursor(0,1);
    lcd.print("Puls: "); lcd.print(pulse);

    delay(500);
  }
}