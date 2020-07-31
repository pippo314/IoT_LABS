
#include <MQTTclient.h>

const int LED_PIN = 7;

int myPub(){
  Process p;
  p.begin("mosquitto_pub");
  p.addParameter("-h");
  p.addParameter("test.mosquitto.org");
  p.addParameter("-p");
  p.addParameter("1883");
  p.addParameter("-t");
  p.addParameter("/tiot/23/sw3");
  p.addParameter("-m");
  p.addParameter("Ciao.");
  p.run();

 Serial.println("msg sent.");
  int ret = p.exitValue();
  return ret;
}


void setup(){
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Starting...");

  digitalWrite(LED_PIN, HIGH);
  Bridge.begin();
  digitalWrite(LED_PIN, LOW);

  mqtt.begin("test.mosquitto.org", 1883);
}

void loop(){
//  mqtt.publish("/test_topic", "Ciao bello.");
  Serial.println(myPub());
  
  delay(2000);
}
