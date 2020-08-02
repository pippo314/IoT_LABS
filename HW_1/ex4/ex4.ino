const int FAN_PIN = 10;
const int step = 25;
float current_speed = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(FAN_PIN, OUTPUT);
  analogWrite(FAN_PIN, (int) current_speed);
  Serial.println("Lab 1.4 start");

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    int b = Serial.read();
    if (b == '+') {//+
      if (current_speed+step >= 255) {
         current_speed=255;
         Serial.println("Max speed.");
         analogWrite(FAN_PIN, (int) current_speed);
      } else {
         current_speed+=step;
         analogWrite(FAN_PIN, (int) current_speed);
        }
    } else if (b == '-') { //-
        if (current_speed-step <= 0) {
          current_speed=0;
          Serial.println("Min speed.");
          analogWrite(FAN_PIN, (int) current_speed);
       } else {
          current_speed-=step;
          analogWrite(FAN_PIN, (int) current_speed);
      }
    } else
    Serial.println("Errore.");
  }
}
