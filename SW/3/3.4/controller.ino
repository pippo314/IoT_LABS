#include <LiquidCrystal_PCF8574.h>
#include <Process.h>
#include <Bridge.h>
#include <ArduinoJson.h>
#include <MQTTclient.h>

LiquidCrystal_PCF8574 lcd(0x27);

const int FAN_PIN = 5;
const int TEMP_PIN = A0;
const int LED_PIN = 9;
const int PIR_PIN = 7;
const int SOUND_PIN = 4;
const int LIGHT_PIN = 12; //lampadina smart

const int B = 4275;
const long int R0 = 100000;

const int timeout_pir = 1800000; //30 minuti
const int timeout_sound = 60000; //se l'ultima rilevazione risale a più di 1 minuto fa non c'è nessuno
const int sound_interval = 500; //campionamento sensore
const int minutes5 = 300000;


//variabili per n_sound_events
boolean first_half = true;
int n_events_first = 0;
int n_events_second = 0;
unsigned long int camp = 0; //ultimo istante di campionamento 

//valori dal seriale e anche i set points
int fan_values[4];
int led_values[4];

//fan setpoints
int fan_min = 20;
int fan_max = 30;


//led setpoints
int led_min = 10;
int led_max = 20;

//lampadina smart
unsigned long int c;
int light_time = 500; //periodo campionamento sensore di suono
int value = LOW; //stato lampadina


unsigned long int last; //ultimo istante in cui si è registrato un interrupt del PIR
unsigned long int ss;
unsigned long int sound_event;

int v_max = 255; //valore massimo di tensione del led e del motore DC

float temp;
int current_speed = 0;
int current_light = 0;
int sound_is_present = 0;
int pir_is_present = 0;
int is_present = 0;
int n_sound_events;
unsigned long int s = 0; //conteggio 10 minuti

//funzioni utilizzate
void setSpeed(float temp); //imposta la velocità della ventola data la temperatura
void setLight(float temp); //imposta la luminosità del led data la temperatura
void PIRisr(); // ISR del sensore di movimento
float readTemp(); // legge il valore delle temperatura dell'ambiente
void printLCD(); //stampa le informazioni del controllore
void turnLightOn(); // accende e spegne il led attraverso battiti di mani
void changeSetPoints(); // cambia i set point
void insertSetPoints(); // legge i valori dei setpoint inseriti da seriale
void readSound(); // rileva persone nella stanza in base al suono
void event(); // conta rilevazioni di suono e incrementa il contatore corrispondente
void PIRtimeout(); // cambia setpoints trascorsi timeiut_pir senza rilevare movimento

unsigned long int mm; //invio valori

Process p;

DynamicJsonDocument doc_snd(JSON_OBJECT_SIZE(3)+ JSON_ARRAY_SIZE(1)+ JSON_OBJECT_SIZE(8));
DynamicJsonDocument doc_rcv(JSON_OBJECT_SIZE(8));

void setup() {
  Serial.begin(9600);
//  while(!Serial);
  Serial.println("Starting...");



  
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(SOUND_PIN, INPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  
  digitalWrite(LED_PIN, HIGH);
  Bridge.begin();
  digitalWrite(LED_PIN, LOW);

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();

  mqtt.begin("test.mosquitto.org", 1883);
  mqtt.subscribe("/tiot/23/sw3/act", decodeCommand);

  registerOnCatalog();
  
  
  last=millis();
  ss=millis();
  mm=millis();
  sound_event=0;

  analogWrite(FAN_PIN, (int) current_speed); //velocità fan parte da 0
  analogWrite(LED_PIN, (int) current_light); //luminosità led parte da 0

  attachInterrupt(digitalPinToInterrupt(PIR_PIN), PIRisr, FALLING);
}

void changeSetPoints(){
    if (is_present == 0) {
        fan_min = fan_values[2];
        fan_max = fan_values[3];
        led_min = led_values[2];
        led_max = led_values[3];
    } else {
        fan_min = fan_values[0];
        fan_max = fan_values[1];
        led_min = led_values[0];
        led_max = led_values[1];
    }
}

void event(){
  if(first_half == true){
    n_events_first++;
  } else n_events_second++;
}

void readSound(){

  if(millis() - sound_event >= timeout_sound){ //60 minuti set is_present
    sound_is_present = 0;
    is_present = pir_is_present | sound_is_present;
    changeSetPoints();
    sound_event = millis();
  }

  if(millis() - ss >= minutes5){//cambio prima e seconda metà
    if(first_half == true){
      n_events_second = 0;
    } else {
      n_events_first = 0;
    }
    first_half=!first_half;
  }

  if(millis() - camp >= sound_interval){ //campionamento 500ms
      if(digitalRead(SOUND_PIN) == LOW) {
      event();
      camp = millis();
    }
  }

  if(millis() - s >= sound_interval){ //intervallo da 10 minuti
    n_sound_events = n_events_first + n_events_second;
    if(n_sound_events > 50) {
        sound_event = millis();
        sound_is_present = 1;
    }
    s = millis();
   } 
}

void loop() {
  mqtt.monitor();
  
  temp = readTemp();

  //invio valori al controller remoto
  if(millis() - mm >= 20000){
      temp=readTemp();
      senMlEncode("t", temp, "Cel");
      senMlEncode("n", sound_event, "null");
      senMlEncode("p", is_present, "null");
      mm = millis();
  }


    setSpeed(temp);
    setLight(temp);
    PIRtimeout();
    readSound();
    printLCD();
    turnLightOn();
}

void mqttRequest(){
  Serial.println(F("Sending request."));
  //F("{"Device\":\"Yun\",\"Resorces\":\"lots\",\"end_points\":\"/tiot/23/sw3/pub\"}")
  Serial.println(mqtt.publish(F("/tiot/23/sw3_4/reg"), F("{\"deviceId\":\"Yun\",\"resources\":\"lots\",\"end_points\":\"/tiot/23/sw3/pub\"}")));
  Serial.println("Sent.");
}

void registerOnCatalog(){
  Serial.println(F("Registering..."));
  delay(5000);
  mqttRequest();
}

void senMlEncode(String res, int v, String unit){
  p.flush();
  doc_snd.clear(); //libera memoria utilizzata da doc_snd
  doc_snd["bn"] = "Yun";
  doc_snd["e"][0]["u"] = unit;
  doc_snd["e"][0]["n"] = res;
  doc_snd["e"][0]["t"] = (long) millis()/1000.0;
  doc_snd["e"][0]["v"] = v;
  
  String output;
  serializeJson(doc_snd, output);
  Serial.println(output);
  mqtt.publish("/tiot/23/sw3/pub", output);
}

void decodeCommand(const String& topic, const String& subtopic, const String& message){
  DeserializationError err = deserializeJson(doc_rcv, message);
  if(err) {
    Serial.println(F("deserialize failed with code: "));
    Serial.println(err.c_str());
  }

  if(doc_rcv["e"][0]["n"] == "led"){
    if (doc_rcv["e"][0]["v"] == 0 || doc_rcv["e"][0]["v"] == 1){
      digitalWrite(7, doc_rcv["e"][0]["v"]); 
    }
    else {
      Serial.println("Value not correct"); 
    }
  }
  else if(doc_rcv["e"][0]["n"] == "fan"){
    if (doc_rcv["e"][0]["v"] == 1){
        analogWrite(A0, 255); 
    }
    else if (doc_rcv["e"][0]["v"] == 0){
      analogWrite(A0, 0); 
      //COSTANTE ELIMINATE E VELOCITA' IMPOSTATA COME "ACCESO O SPENTO"
    }
    else {
        Serial.println("Value not correct"); 
    }
  }
  else if(doc_rcv["e"][0]["n"] == "print"){
    lcd.home();
    String str = doc_rcv["e"][0]["s"];
    lcd.print(str);
  }
  else if(doc_rcv["e"][0]["n"] == "setpoints"){
    setpoints2();
  }
}

void setpoints2(){
  int sl = doc_rcv["e"][0]["sl"]; // ex 12345678
  int sf = doc_rcv["e"][0]["sf"]; // ex 09877655
  for (int i = 0; int l = 3; i < 4 && l >= 0, i++, l--) {
    fan_values[l] = sf % 100;
    sf = (int) sf/100;
    led_values[l] = sl % 100;
    sl = (int) sl/100;
  }
  changeSetPoints(); //questa funzione modifica i set points in base alla presenza o meno di una persona
}

void PIRtimeout(){
      if(millis() - last >= timeout_pir){
      pir_is_present = 0;
      is_present = pir_is_present | sound_is_present;
      changeSetPoints();
      last = millis();
    }
}

void setSpeed(float temp){
  int current_speed;
  if(temp > fan_min && temp < fan_max){ //temperatura in range
      current_speed = v_max * temp / fan_max;
  } else if(temp < fan_min) {
      current_speed = 0;
  } else {
      current_speed = 255;
  }
    analogWrite(FAN_PIN, (int) current_speed);
}

void setLight(float temp){
    if(temp > led_min && temp < led_max) { //temperatura in range
        current_light = v_max * (1 - (temp / led_max));
    } else if(temp < led_min){
        current_light = 255;
    } else {
        current_light = 0;
    }
    analogWrite(LED_PIN, (int) current_light);
}

void PIRisr(){
    pir_is_present = 1;
    last = millis();
}

float readTemp(){
    int sig = analogRead(TEMP_PIN);
    float R = ((1023.0/sig) - 1.0);
    R = (float) R * R0;
    float log_sig = log(R/R0);
    float temp = 1/((log_sig/B) + 1/298.15) - 273.15;
    return temp;
}

/*void insertSetPoints(){
  Serial.println("Inserire valori per Riscaldamento (prima valori per presente): ");
  int i=0;
  while(i<4){
    if(Serial.available()>0){
      int inByte = Serial.parseInt();
      fan_values[i]=inByte;
      i++;
    }
  }

  Serial.println("Inserire valori per Condizionamento (prima valori per presente): ");
  i=0;
  while(i<4){
    if(Serial.available()>0){
      int inByte = Serial.parseInt();
      led_values[i]=inByte;
      i++;
    }
  }
}*/

void printLCD(){
  lcd.home();
  lcd.print("T:");
  lcd.print(temp);
  lcd.print(" Pres:");
  lcd.print(is_present);
  lcd.setCursor(0,1);
  int fan = current_speed * 100 / 255;
  int led = current_light * 100 / 255;
  lcd.print("AC:");
  lcd.print(fan);
  lcd.print("% ");
  lcd.print("HT:");
  lcd.print(led);
  lcd.print("%");

  delay(5000);
  lcd.clear();
  lcd.home();
  lcd.print("AC m:");
  lcd.print(fan_min);
  lcd.print(" M:");
  lcd.print(fan_max);
  lcd.setCursor(0,1);
  lcd.print("HT m:");
  lcd.print(led_min);
  lcd.print(" M:");
  lcd.print(led_max);
  delay(5000);
}

void turnLightOn(){
    int n=0;
  if(millis() - c >= light_time){
      for(int i=0; i < 4; i++){
          if(digitalRead(SOUND_PIN) == LOW){
              delay(400);
              n++;
          }
      }
      if(n == 2){
          value=!value;
          digitalWrite(LIGHT_PIN, value);
      }
      c = millis();
  }
	if (!is_present){
		digitalWrite(LIGHT_PIN,LOW);
		}
}
