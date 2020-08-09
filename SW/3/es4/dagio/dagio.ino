#include <Process.h>
#include <Bridge.h>
#include <ArduinoJson.h>
#include <MQTTclient.h>
#include <LiquidCrystal_PCF8574.h>


LiquidCrystal_PCF8574 lcd(0x27);

//const int LED_PIN = 7;
//const int FAN_PIN = A0;
// QUESTE NON SONO INDISPENSABILI, CI BASTA COMMENTARLE E SCRIVERE IL NUMERO

unsigned long int mm;

int current_speed = 160;

int temp = 4;
int fan_values[4];
int led_values[4];
int sound_event = 5;
int is_present = 0;
Process p;

/*
const String pub_topic = "/tiot/23/sw3/pub";
const String sub_topic = "/tiot/23/sw3/sub";
const String catalog_topic="/tiot/23/catalog"
VALE LO STESSO CHE PER I PIN
*/


/*
 * 
 *    msg = {
      "bn": "ArduinoYun", 
      "e": [
        {
        "n": "temperature", 
        "t": "null", 
        "v": 0, 
        "u": "null",
          }
        ]
        }
      
 */
 
        

/*FILE MQTT IN RICEZIONE

msg =
        {
        "n": "LED,FAN,PRINT,SETPOINT", 
        "v": 0, 
        "sl": 21314151,
        "sf":42567830
          }
  sl e sf rappresentano i setpoint del led e del fan rispettivamente
  vengono divisi in 4 variabili ciascuno con divisioni per 100 successive
*/

DynamicJsonDocument doc_snd(JSON_OBJECT_SIZE(3)+ JSON_ARRAY_SIZE(1)+ JSON_OBJECT_SIZE(8));
DynamicJsonDocument doc_rcv(JSON_OBJECT_SIZE(6));
//ELIMINATE LE COSTANTI PER LA DEFINIZIONE DELLE CAPACITA'

void changeSetPoints();

void setup(){
  pinMode(7, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Starting...");

  digitalWrite(7, HIGH);
  Bridge.begin();
  digitalWrite(7, LOW);

  mm = millis();

  mqtt.begin("test.mosquitto.org", 1883);
  registerOnCatalog();

//  lcd.begin(16, 2);
//  lcd.setBacklight(255);
//  lcd.home();
//  lcd.clear();

  mqtt.subscribe("/tiot/23/sw3/act", decodeCommand);
}

void loop(){
  mqtt.monitor();
//  if(millis() - mm >= 20000){
//      temp=readTemp();
//      senMlEncode("t", temp, "Cel");
//      senMlEncode("n", sound_event, "null");
//      senMlEncode("p", is_present, "null");
//      mm = millis();
//  }
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

/*void setpointsFromMQTT(){
 //     Serial.println(doc_rcv["e"][0]["s"]);
  int i,p=0,lol[2];
  int vett[4];
  boolean x=0;
  for (i=0;i<13;i++){
    if((i==12) || (doc_rcv["e"][0]["s"][i]=='q')){
      if(i==12){
        Serial.println("Errore nei dati.");
        return;
      }
      vett[p]=lol[0]*10+lol[1];
      p++;
      if (p==4)
        x=1;
      if (x){
        for (i=0;i<4;i++)
          fan_set_points[i]=vett[i];
        Serial.println(F("Fan_set_points modificati correttamente"));
        return;
      }
      else{
        Serial.println("errore nei dati");
        return;
      }
    }
    else if(doc_rcv["e"][0]["s"][i]==' '){
      vett[p]=lol[0]*10+lol[1];
      p++;
      lol[0]=0;
      lol[1]=0;
    }
    else if(doc_rcv["e"][0]["s"][i]!=' '){
      if(lol[1]==0)
        lol[1]=doc_rcv["e"][0]["s"][i];
      else {
        lol[0]=lol[1];
        lol[1]=doc_rcv["e"][0]["s"][i];
      }
    }
  }
}*/

/*implementato come segue: il valore massimo del vettore di caratteri s in arrivo
e' di 11 elementi di cui 8 numeri e 3 spazi. di piu' non avrebe senso. ho inserito il carattere terminatore q,
adesso non ricordo il motivo ma sembrava una buona idea, per cui i caratteri in totale sono 12.
tenendo conto che in c/c++ i char sono anche int e' possibile eseguire operazioni matematiche su di essi 
anche perche sono castati dalla lettura di lol. comunque il primo numero letto viene inserito in lol[1] che rappresenta le unita'. se viene trovato un secondo numero affiancato al primo allora il valore precedente viene trasferito in lol[0] contenitore delle unita', mentre in lol[1] viene inserito il nuovo valore. 
se viene raggiunto il limite senza alcun terminatore "q" il ciclo finisce male, se gli spazi non sono corretti allora il ciclo finisce male, se invece viene inserito tutto bene allora il ciclo si conclude con il salvataggio dai set_points.
*/

/*void senMlEncode(String res, int v, String unit){
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

void registerOnCatalog(){
  Serial.println(F("Registering..."));
  delay(5000);
  mqttRequest();
}

void mqttRequest(){
  Serial.println(F("Sending request."));
  //F("{"Device\":\"Yun\",\"Resorces\":\"lots\",\"end_points\":\"/tiot/23/sw3/pub\"}")
  Serial.println(mqtt.publish(F("/tiot/23/sw3_4/reg"), F("{\"deviceId\":\"Yun\",\"resources\":\"lots\",\"end_points\":\"/tiot/23/sw3/pub\"}")));
  Serial.println("Sent.");
}

float readTemp(){
    float R = ((1023.0/analogRead(A1)) - 1.0);
    R = R * 100000;
    float temp = 1/((log(R/100000)/4275) + 1/298.15) - 273.15;
    return temp;
}*/
