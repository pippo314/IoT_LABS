#include <Process.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>
#include <MQTTclient.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

const int LED_PIN = 7;
const int FAN_PIN = A0;

unsigned long int mm;

int current_speed = 160;

int temp = 4;
int sound_event = 5;
int is_present = 0;

const String topic = "/tiot/23/sw3";

const String catalog_Address = "http://192.168.1.52:8080/devices/add";

//const int capacity = JSON_OBJECT_SIZE(2)+JSON_ARRAY_SIZE(1)+JSON_OBJECT_SIZE(4)+40;
//const int capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3);
//const int catalog_capacity = JSON_ARRAY_SIZE(1)+JSON_OBJECT_SIZE(3);
//const int capacity = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(7);
const int capacity = 60;

void Update(){ 
  Process p; 
  p.begin("opkg"); 
  p.addParameter("update"); 
  p.run(); p.flush(); 
} 
void Pack(){ 
  Process p; 
  p.begin("opkg"); 
  p.addParameter("install mosquitto-client libmosquitto"); 
  p.run(); 
  p.flush(); 
} 

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
DynamicJsonDocument doc(capacity);
//DynamicJsonDocument doc_rcv(capacity);


void setup(){
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Starting...");

  digitalWrite(LED_PIN, HIGH);
  Bridge.begin();
  digitalWrite(LED_PIN, LOW);
//  Update();
//  Pack();

  mm = millis();

//  Serial.println(capacity);
  int n = registerOnCatalog();
  Serial.println(n);
  mqtt.begin("test.mosquitto.org", 1883);
//  mqtt.subscribe(topic, decodeCommand());

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
}

void loop(){
//  serialFlush();
  if(millis() - mm >= 5000){
      sendValues("t", temp, "Cel");
      sendValues("n", sound_event, "null");
      sendValues("p", is_present, "null");
      mm = millis();
    }
}

void serialFlush(){ 
  char t='a';
  while(Serial.available() > 0) { 
      t = Serial.read(); 
      Serial.println(t);
    }  
} 

void runFree(){
  Process p;
  p.begin("free");
   p.run();    // Run the process and wait for its termination

  // Print arduino logo over the Serial
  // A process output can be read with the stream methods
  while (p.available() > 0) {
    char c = p.read();
    SerialUSB.print(c);
  }
  // Ensure the last bit of data is sent.
  SerialUSB.flush();
}
 
void decodeCommand(const String& topic, const String& subtopic, const String& message){
    DeserializationError err = deserializeJson(doc, message);
  if(err) {
    Serial.println(F("deserialize failed with code: "));
    Serial.println(err.c_str());
  }

  if(doc["e"][0]["n"] == "led"){
    if (doc["e"][0]["v"] == 0 || doc["e"][0]["v"] == 1) { 
      digitalWrite(LED_PIN, doc["e"][0]["v"]); 
      } else { 
        Serial.println("Value not correct"); 
      } 
  }
  else if(doc["e"][0]["n"] == "fan"){
    if (doc["e"][0]["v"] == 0 || doc["e"][0]["v"] == 1) { 
      int value = doc["e"][0]["v"];
      analogWrite(FAN_PIN, (int) current_speed * value); 
      } else { 
        Serial.println("Value not correct"); 
      } 
  }
  else if(doc["e"][0]["n"] == "print"){
    lcd.home();
    char* msg = doc["e"][0]["s"];
    lcd.print(msg);
  }
  else if(doc["e"][0]["n"] == "setpoints"){
    char* setpoints = doc["e"][0]["s"];
//    int len = 7;
//    char* buf = malloc(sizeof(char) * len);
    Serial.println(setpoints);
//    setpoints.toCharArray(buf, len);
  }
}

String senMlEncode(String res, float v, String unit){
  doc.clear(); //libera memoria utilizzata da doc_snd
  doc["bn"] = "Yun";
  doc["e"][0]["u"] = unit;
  doc["e"][0]["n"] = res;
  doc["e"][0]["t"] = (int) millis()/1000.0;
  doc["e"][0]["v"] = v;
  
  String output;
  serializeJson(doc, output);
  Serial.println(output);
  return output;
}

void sendValues(String s, int n, String c){

  String message = senMlEncode(s, n, c);
  Serial.println(mqtt.publish(topic, message));
  mqtt.publish(topic, message);
//  message = senMlEncode("t", temp, "Cel");
//  message = senMlEncode("n", sound_event, "null");
//  mqtt.publish(topic, message);
//  message = senMlEncode("p", is_present, "null");
//  mqtt.publish(topic, message);
}



int registerOnCatalog(){
  Serial.println("Registering...");
//  int len = 2;
//  String end_points[len] = { "/tiot/23/sw3", "altro_topic" };
//  String json = catalogEncode("Yun", "sensori", end_points, len);
//  Serial.println(json);
//  runFree();
  delay(5000);
  return putRequest();
}

/*String catalogEncode(String deviceId, String resources, String *end_points, int len){
  Serial.println("encode");
  doc.clear(); //libera memoria utilizzata da doc
  doc["deviceId"] = deviceId;
  doc["resources"] = resources;
  doc["end_points"] = "/tiot/23/sw3";
//  for(int i = 0; i < len; i++){
//    doc_snd["end_points"][i] = end_points[i];
//    Serial.println("nel for");
//  }
  
  String output;
  serializeJson(doc, output);
//  runFree();
  return output;
}*/

int putRequest(){
  Serial.println(F("Sending request."));
//  Serial.print(data);
  Process p;
//  p.runShellCommand(F("curl -H Content-Type: application/json -X -POST -d Ciao http://192.168.1.52:8080/devices/add"));
  p.begin("curl");
  p.addParameter(F("-H"));
  p.addParameter(F("Content-Type: application/json"));
  p.addParameter(F("-X"));
  p.addParameter(F("PUT"));
  p.addParameter(F("-d"));
  p.addParameter(F("{\"deviceId\":\"Yun\", \"resources\":\"sensori\", \"end_points\":\"/tiot/23/sw3\"}"));
  p.addParameter(catalog_Address);
  p.run();
  while (p.running());
  while (p.available() > 0) {
    char c = p.read();
    Serial.print(c);
  }
  // Ensure the last bit of data is sent.
  Serial.flush();
  Serial.println("\nSent.");
  int ret = p.exitValue();
//  p.flush();
  p.close();
  return ret;
}
