#include <Process.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>

BridgeServer server;

//parametri sensore temperatura
const int B = 4275;
const long int R0 = 100000;
const int TEMP_PIN = A0;

const int capacity = JSON_OBJECT_SIZE(2)+JSON_ARRAY_SIZE(1)+JSON_OBJECT_SIZE(4)+40;
DynamicJsonDocument doc_snd(capacity);

// temperatura
float readTemp(){
    int sig = analogRead(TEMP_PIN);
    float R = ((1023.0/sig) - 1.0);
    R = (float) R * R0;
    float log_sig = log(R/R0);
    float temp = 1/((log_sig/B) + 1/298.15) - 273.15;
    return temp;
}



int postRequest(String data){
	Process p;
	p.begin("curl");
	p.addParameter("-H");
	p.addParameter("Content-Type: application/json");
	p.addParameter("_X");
	p.addParameter("POST");
	p.addParameter("-d");
	p.addParameter(data);
	p.addParameter("http://192.168.1.52:8080/devices/add");
	p.run();

	return p.exitValue();
}


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



void setup()
{
	Bridge.begin();
  	server.listenOnLocalhost();
  	server.begin();
}

void loop()
{
	float temp_val = readTemp();
	String output;
	output = senMlEncode(F("temperature"), temp_val, F("Cell"));
	postRequest(output);
  delay(1000);
}
