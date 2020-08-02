#include <TimerOne.h>

const int RLED_PIN = 12;
const int GLED_PIN = 11;

const float R_HALF_PERIOD = 1.5;
const float G_HALF_PERIOD = 3.5;

int greenLedState = LOW;
int redLedState = LOW;


void setup() {

  Serial.begin(9600);
  while (!Serial);
  pinMode(RLED_PIN, OUTPUT);
  pinMode(GLED_PIN, OUTPUT);
  Serial.println("Lab 1.2 starting");
  Timer1.initialize(G_HALF_PERIOD * 1e06);
  Timer1.attachInterrupt(blinkGreen);
}

void blinkGreen(){
  greenLedState = !greenLedState;
  digitalWrite(GLED_PIN, greenLedState);
}



void serialPrintStatus(){
  if(Serial.available()>0){
    int inByte = Serial.read();

    if (inByte == 'R'){
      Serial.print("LED 1 Status: ");
      Serial.println(redLedState);
    }else if (inByte == 'G'){
      Serial.print("LED 2 Status: ");
      Serial.println(greenLedState);
    }else {
      Serial.print("Invalid command\n");
    }
  }
}


void loop(){
  serialPrintStatus();
  redLedState = !redLedState;
  digitalWrite(RLED_PIN, redLedState);
  delay(R_HALF_PERIOD * 1e03);
  
}
