
#include <Arduino.h>

int counter = 0; 

void setup(){
  Serial.begin(9600); 
}


void loop() {
  counter++; 

  if(counter % 3 == 0)
  {
    if (counter % 5 == 0)
    {
      Serial.println("Fizz Buzz"); 
    }
    else
    {
      Serial.println("Fizz"); 
    }
  }
  else
  {
    if (counter % 5 == 0)
    {
      Serial.println("Buzz"); 
    }
    else
    {
      Serial.println(counter); 
    }
  }
  delay(500); 
}
