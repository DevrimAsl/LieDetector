#include <Arduino.h>
#include <Bounce2.h>

const int BTN_YLW = 2;
const int BTN_BlU = 3;
const int BTN_GRN = 4;

const int LED_GRN = 8;
const int LED_PNK = 9;

Bounce2::Button btnYellow = Bounce2::Button();
Bounce2::Button btnBlue = Bounce2::Button();
Bounce2::Button btnGruen = Bounce2::Button();

void setup() {

  Serial.begin(9600);

  pinMode(LED_GRN, OUTPUT);
  pinMode(LED_PNK, OUTPUT);

  btnYellow.attach(BTN_YLW, INPUT_PULLUP);
  btnYellow.setPressedState(LOW);

  btnBlue.attach(BTN_BlU, INPUT_PULLUP);
  btnBlue.setPressedState(LOW);

  btnGruen.attach(BTN_GRN, INPUT_PULLUP);
  btnGruen.setPressedState(LOW);
}

void loop() {

  btnYellow.update();
  btnBlue.update();
  btnGruen.update();

  if (btnYellow.pressed()) {
    // S = 3x kurz
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_PNK, HIGH);
        delay(300);          // kurz
        digitalWrite(LED_PNK, LOW);
        delay(300);
    }

    // kleine Pause zwischen Buchstaben
    delay(600);

    // O = 3x lang
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_PNK, HIGH);
        delay(900);          // lang
        digitalWrite(LED_PNK, LOW);
        delay(300);
    }

    // Pause zwischen Buchstaben
    delay(600);

    // S = 3x kurz
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_PNK, HIGH);
        delay(300);          // kurz
        digitalWrite(LED_PNK, LOW);
        delay(300);
    }
  }

  if (btnGruen.pressed()) {

    for (int i = 0; i < 5; i++) 
    {
      digitalWrite(LED_GRN, HIGH);
      delay(300);
      digitalWrite(LED_GRN, LOW);
      delay(300);
    }
  }
   if(btnBlue.pressed())
  {
    for (int i = 0; i < 9; i++)
    {
      digitalWrite(LED_PNK, HIGH);
      delay(500);
      digitalWrite(LED_PNK, LOW);
      delay(500);
      digitalWrite(LED_GRN, HIGH);
      delay(500);
      digitalWrite(LED_GRN, LOW);
      delay(500);

    } 
  }
}