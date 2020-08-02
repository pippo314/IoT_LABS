#include <MQTTclient.h>
#include <ArduinoJson.h>
#include <Bridge.h>
#include <Process.h>


//parametri sensore temperatura
const int B = 4275;
const long int R0 = 100000;
const int TEMP_PIN = A0;

const int LED_PIN = 7;

const int capacity = JSON_OBJECT_SIZE(2)+JSON_ARRAY_SIZE(1)+JSON_OBJECT_SIZE(4)+40;
DynamicJsonDocument doc_snd(capacity);
//DynamicJsonDocument doc_rcv(capacity);

const String topic_publish = "/tiot/23/temperature/";
const String topic_subscribe = "/tiot/23/led";

// temperatura
float readTemp(){
    int sig = analogRead(TEMP_PIN);
    float R = ((1023.0/sig) - 1.0);
    R = (float) R * R0;
    float log_sig = log(R/R0);
    float temp = 1/((log_sig/B) + 1/298.15) - 273.15;
    return temp;
}

/*void setLedValue(const String& topic, const String& subtopic, const String& message) {

    // print the topic and message
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("message: ");
  Serial.println(message);

  DeserializationError err = deserializeJson(doc_rcv, message);
  if(err) {
    Serial.print(F("deserialize failed with code: "));
    Serial.print(err.c_str());
  }
  if(doc_rcv["e"][0]["n"] == "led"){
    Serial.print("if");
    if (doc_rcv["e"][0]["v"] == 0 || doc_rcv["e"][0]["v"] == 1) {
      Serial.print("primo if");
      digitalWrite(LED_PIN, doc_rcv["e"][0]["v"]);
      } else {
        Serial.println("Value not correct");
      }
  }
}*/

String senMlEncode(String res, float v, String unit){
  doc_snd.clear(); //libera memoria utilizzata da doc_snd
  doc_snd["bn"] = "Yun";
  doc_snd["e"][0]["u"] = unit;
  doc_snd["e"][0]["n"] = res;
  doc_snd["e"][0]["t"] = millis()/1000.0;
  doc_snd["e"][0]["v"] = v;
  String output;
  serializeJson(doc_snd, output);
  return output;
}


void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  digitalWrite(LED_PIN, LOW);
  Bridge.begin();
  digitalWrite(LED_PIN, HIGH);
  mqtt.begin("test.mosquitto.org", 1883);
//  mqtt.subscribe(topic_subscribe, setLedValue);
}

void loop() {
  mqtt.monitor();
  //status();
  float temp_val = readTemp();
  String message;
  message = senMlEncode(F("temperature"), temp_val, F("Cell"));
  Serial.println(message);
  mqtt.publish("/tiot/23/temperature", message);

  delay(1000);
}
