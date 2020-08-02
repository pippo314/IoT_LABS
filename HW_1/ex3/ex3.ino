const int LED_PIN = 11;
const int PIR_PIN = 7;
volatile int tot_count = 0;


void checkPresence(){
  int pir = digitalRead(PIR_PIN);
  if (pir){
    
    tot_count += 1;
    digitalWrite(LED_PIN, HIGH);
    delay(10000);
    digitalWrite(LED_PIN, LOW);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), checkPresence, CHANGE);
}


void loop()
{
  Serial.println(tot_count);
  delay(30000);
}
