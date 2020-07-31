#include <Process.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>
#include <MQTTclient.h>
//#include <LiquidCrystal_PCF8574.h>

const int LED_PIN = 7;
const int FAN_PIN = A0;

const String topic = "/tiot/23/sw3";

const String catalog_Address = "http://192.168.1.52:8080/devices/add";

const int capacity = JSON_OBJECT_SIZE(6);

DynamicJsonDocument doc_snd(capacity);


void setup(){
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Starting...");

  digitalWrite(LED_PIN, HIGH);
  Bridge.begin();
  digitalWrite(LED_PIN, LOW);

  mqtt.begin("test.mosquitto.org", 1883);
}

void loop(){
  String msg = "Ciao.";
  mqtt.publish(topic, msg);
  Serial.println("Sent.");
  delay(5000);
  }

int registerOnCatalog(){
  Serial.println("Registering...");
  int len = 2;
  String end_points[len] = { "/tiot/23/sw3", "altro_topic" };
  String json = catalogEncode("Yun", "sensori", end_points, len);
  Serial.println(json);
  return putRequest(json);
}

String catalogEncode(String deviceId, String resources, String *end_points, int len){
  Serial.println("encode");
  doc_snd.clear(); //libera memoria utilizzata da doc_snd
  doc_snd["deviceId"] = deviceId;
  doc_snd["resources"] = resources;
  doc_snd["end_points"] = "/tiot/23/sw3";
  
  String output;
  serializeJson(doc_snd, output);
  return output;
}

int putRequest(String data){
  Serial.println("Sending request.");
  Serial.print(data);
  Process p;
  p.begin("curl");
//  p.addParameter("-v");
  p.addParameter("-H");
  p.addParameter("Content-Type: application/json");
  p.addParameter("-X");
  p.addParameter("POST");
  p.addParameter("-d");
  p.addParameter(data);
  p.addParameter(catalog_Address);
  p.run();
    while (p.available() > 0) {
    char c = p.read();
    Serial.print(c);
  }
  // Ensure the last bit of data is sent.
  Serial.flush();
  Serial.println("Sent.");
  int ret = p.exitValue();
  p.flush();
  p.close();
  return ret;
}
