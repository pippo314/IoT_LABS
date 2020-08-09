#include <Process.h>
#include <Bridge.h>
#include <ArduinoJson.h>
#include <MQTTclient.h>
#include <LiquidCrystal_PCF8574.h>
Process p;

LiquidCrystal_PCF8574 lcd(0x27);

const int LED_PIN = 7;
const int FAN_PIN = A0;

unsigned long int mm;

int current_speed = 160;

int temp = 4;
int sound_event = 5;
int is_present = 0;

const String pub_topic = "/tiot/23/sw3/pub";
const String sub_topic = "/tiot/23/sw3/sub";

const String catalog_Address = "http://192.168.1.13:8080/devices/add";

//const int capacity = JSON_OBJECT_SIZE(2)+JSON_ARRAY_SIZE(1)+JSON_OBJECT_SIZE(4)+40;
//const int capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3);
//const int catalog_capacity = JSON_ARRAY_SIZE(1)+JSON_OBJECT_SIZE(3);
const int capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4)+102;
//const int capacity = 55;


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
        "s": "2 3 4 5"
          }
        ]
        }
      
 */

//StaticJsonDocument<capacity>doc_snd;
DynamicJsonDocument doc_snd(capacity);
DynamicJsonDocument doc_rcv(capacity);
//DynamicJsonDocument doc_rcv(capacity);


void setup(){
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Starting...");

  digitalWrite(LED_PIN, HIGH);
  Bridge.begin();
  digitalWrite(LED_PIN, LOW);

  mm = millis();

//  int n = registerOnCatalog();
//  Serial.println(n);
  mqtt.begin("test.mosquitto.org", 1883);
  mqtt.subscribe(sub_topic, decodeCommand);

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
}

void loop(){
  mqtt.monitor();
  if(millis() - mm >= 20000){
      sendValues("t", temp, "Cel");
      sendValues("n", sound_event, "null");
      sendValues("p", is_present, "null");
      mm = millis();
    }
}

 
void decodeCommand(const String& topic, const String& subtopic, const String& message){
  doc_rcv.clear();
  DeserializationError err = deserializeJson(doc_rcv, message);
  if(err) {
    Serial.println(F("deserialize failed with code: "));
    Serial.println(err.c_str());
  }

  if(doc_rcv["e"][0]["n"] == "led"){
    if (doc_rcv["e"][0]["v"] == 0 || doc_rcv["e"][0]["v"] == 1) { 
      digitalWrite(LED_PIN, doc_rcv["e"][0]["v"]); 
      } else { 
        Serial.println("Value not correct"); 
      } 
  }
  else if(doc_rcv["e"][0]["n"] == "fan"){
    if (doc_rcv["e"][0]["v"] == 0 || doc_rcv["e"][0]["v"] == 1) { 
      int value = doc_rcv["e"][0]["v"];
      analogWrite(FAN_PIN, (int) current_speed * value); 
      } else { 
        Serial.println("Value not correct"); 
      } 
  }
  else if(doc_rcv["e"][0]["n"] == "print"){
    lcd.home();
    char* msg = doc_rcv["e"][0]["s"];
    lcd.print(msg);
  }
  else if(doc_rcv["e"][0]["n"] == "setpoints"){
    char* setpoints = doc_rcv["e"][0]["s"];
//    int len = 7;
//    char* buf = malloc(sizeof(char) * len);
    Serial.println(setpoints);
//    setpoints.toCharArray(buf, len);
  }
}

String senMlEncode(String res, float v, String unit){
  doc_snd.clear(); //libera memoria utilizzata da doc_snd_snd
  doc_snd["bn"] = "Yun";
  doc_snd["e"][0]["u"] = unit;
  doc_snd["e"][0]["n"] = res;
  doc_snd["e"][0]["t"] = (long) millis()/1000.0;
  doc_snd["e"][0]["v"] = v;
  
  String output;
  serializeJson(doc_snd, output);
  Serial.println(output);
  return output;
}

void sendValues(String s, int n, String c){
  String message = senMlEncode(s, n, c);
  Serial.println(mqtt.publish(pub_topic, message));
//  mqtt.publish(pub_topic, message);
}

/*int registerOnCatalog(){
  Serial.println(F("Registering..."));
  delay(5000);
  return putRequest();
}*/

/*int putRequest(){
  Serial.println(F("Sending request."));
//  Serial.print(data);
  Process p;
//  p.runShellCommand(F("curl -H Content-Type: application/json -X -POST -d Ciao http://192.168.1.52:8080/devices/add"));
  p.begin(F("curl"));
  p.addParameter(F("-H"));
  p.addParameter(F("Content-Type: application/json"));
  p.addParameter(F("-X"));
  p.addParameter(F("PUT"));
  p.addParameter(F("-d"));
  p.addParameter(F("{\"deviceId\":\"Yun\", \"resources\":\"sensori\", \"end_points\":\"/tiot/23/sw3\"}"));
  p.addParameter(catalog_Address);
  p.run();
  // Ensure the last bit of data is sent.
  Serial.flush();
  Serial.println("\nSent.");
  int ret = p.exitValue();
  p.flush();
  p.close();
  return ret;
}*/
