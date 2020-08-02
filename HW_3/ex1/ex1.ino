#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>

const int B = 4275;
const long int R0 = 100000;
const int TEMP_PIN = A0;


BridgeServer server;
const int LED = 7;
int led_value = LOW;

const int capacity = JSON_OBJECT_SIZE(2)+JSON_ARRAY_SIZE(1)+JSON_OBJECT_SIZE(4)+40;
DynamicJsonDocument doc_snd(capacity);

String senMlEncode(String res, float v, String unit){
  doc_snd.clear(); //libera memoria utilizzata da doc_snd
  doc_snd["bn"] = "Yun";
  if (unit != ""){
    doc_snd["e"][0]["u"] = unit;
  } else{
    doc_snd["e"][0]["u"] = (char*)NULL;
  }
  if (res != ""){
    doc_snd["e"][0]["n"] = res;
  } else{
    doc_snd["e"][0]["n"] = (char*)NULL; // mette solo null al posto di temp/led
  }
  doc_snd["e"][0]["t"] = millis();
  doc_snd["e"][0]["v"] = v;

  String output;
  serializeJson(doc_snd, output);
  return output;
}


void printResponse(BridgeClient client, int status, String body){
  client.println("Status: "+String(status));
  if (status == 200){
    client.println(F("Content-type: application/json; charset=utf-8"));
    client.println();
    client.println(body);
  }
}


void printErrResponse(BridgeClient client, int status){
  client.println("Status: "+String(status));
  client.println();
  client.println("Expected value 0 or 1, try to connect again");
}

void printTempResponse(BridgeClient client, int status){
  client.println("Status: "+String(status));
  client.println();
  client.println("Temperature detected lower than absolute zero");
}

void printBadCommand(BridgeClient client, int status){
  client.println("Status: "+String(status));
  client.println();
  client.println("Expected command 'led' or 'temperature', try to connect again");
}


// temperatura
float readTemp(){
    int sig = analogRead(TEMP_PIN);
    float R = ((1023.0/sig) - 1.0);
    R = (float) R * R0;
    float log_sig = log(R/R0);
    float temp = 1/((log_sig/B) + 1/298.15) - 273.15;
    return temp;
}



void process(BridgeClient client){ 
  String command = client.readStringUntil('/'); //vado a dividere l'URL partendo da dopo /arduino/
  command.trim();

  if (command == "led"){
    int val = client.parseInt(); //leggo nel buffer del bridge client
    if (val == 0 || val == 1){
      digitalWrite(LED, val);
      printResponse(client, 200, senMlEncode(F("led"), val, F("")));
    }else{
      printErrResponse(client, 400);
    }
  }else if (command == "temperature"){
    float temp_val = readTemp();
    if (temp_val > -273.15){
      printResponse(client, 200, senMlEncode(F("temperature"), temp_val, F("Cel")));
    }else{
      printTempResponse(client, 400);
    }
  }else{
    printBadCommand(client, 400);
  }
}

void setup(){
  pinMode(LED, OUTPUT);
  digitalWrite(LED, led_value);
  Bridge.begin();
  digitalWrite(LED, !led_value);
  server.listenOnLocalhost();
  server.begin();
}


void loop(){
  BridgeClient client = server.accept(); //accetto nuovi client che si stanno connettendo 

  if (client){
    process(client); //vado ad elaborare richiesta del client
    client.stop(); //funzione da scrivere
  } 
  delay(50);
}
