#include <math.h>

const int PIN = A5;
const int B = 4275;
const long int R0 = 100000;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 1.5 start.");
}

void loop() {
  int sig = analogRead(PIN);
  float R = ((1023.0/sig) - 1.0);
  R = (float) R * R0;
  float log_sig = log(R/R0);
  float temp = 1/((log_sig/B) + 1/298.15) - 273.15;
  Serial.print("Temperature now: ");
  Serial.println(temp);
  delay(10000);
}
