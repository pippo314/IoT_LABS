#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);

const int TEMP_PIN = A0;
const int B = 4275.0;
const long int R0 = 100000.0;

void setup() {
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  lcd.print("Temperature:");
}

void loop() {
  int sig = analogRead(TEMP_PIN);
  float R = ((1023.0/sig) - 1.0);
  R = (float) R * R0;
  float log_sig = log(R/R0);
  float temp = 1/((log_sig/B) + 1/298.15) - 273.15;
  lcd.setCursor(12,0);
  lcd.print(temp,1);
  delay(2000);

}
