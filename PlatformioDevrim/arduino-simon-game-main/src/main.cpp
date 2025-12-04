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
    Serial.println("Yellow has been pressed");
    digitalWrite(LED_GRN, HIGH);
    delay(2000);
    digitalWrite(LED_GRN, LOW);
  }

  if (btnBlue.pressed()) {
    Serial.println("Blue has been pressed");
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
}