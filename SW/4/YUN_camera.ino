#include <swRTC.h>
#include <Process.h>
#include <Bridge.h>
#include <ArduinoJson.h>
#include <MQTTclient.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

swRTC rtc;
const byte WITHOUT_SECONDS = 0;
const byte WITH_SECONDS = 1;

const int buzzerPin = 2;
const int TEMP_PIN = A0;
const int LED_PIN = 7;
const int FAN_PIN = 5;

unsigned long int mm;
Process p;


/*
topic 1 --> ricezione da server
topic 2 --> invio server 
1) cambiamento is_present
2) inviare orario per richieste quali serrande, boiler, inviare temperatura ed orario per l'altro arduino al server
*/

const int B = 4275;
const long int R0 = 100000;


int fan_values[4];
int led_values[4];

int fan_min = 20;
int fan_max = 25;

int led_min = 15;
int led_max = 25;

int value = LOW; //stato lampadina

int v_max = 255; //valore massimo di tensione del led e del motore DC

float temp;
int current_speed = 0;
int current_light = 0;
unsigned long int c;

int is_present = 0;

void setSpeed(float temp); //imposta la velocità della ventola data la temperatura
void setLight(float temp); //imposta la luminosità del led data la temperatura
float readTemp(); // legge il valore delle temperatura dell'ambiente
void printLCD(); //stampa le informazioni del controllore
void changeSetPoints(); // cambia i set point

DynamicJsonDocument doc_snd(JSON_OBJECT_SIZE(3)+ JSON_ARRAY_SIZE(1)+ JSON_OBJECT_SIZE(8));
DynamicJsonDocument doc_rcv(JSON_OBJECT_SIZE(8));

/*
1) --> "/tiot/23/sw3/sub"
msg=
	{
	"p":value
	"v"=0
	"set1": intero_di_8_cifre
	"set2": intero_di_8_cifre
	}	

2) --> "/tiot/23/sw3/pub"
msg = {
      "bn": nome, 
      "e": [
        {
        "n": "temperatura,serrande oppure boiler", 
        "t": "null", 
        "v": 0, 
        "u": "null",
          }
        ]
        }
}


const String pub_topic = "/tiot/23/sw3/pub";
const String sub_topic = "/tiot/23/sw3/sub";
const String catalog_topic="/tiot/23/catalog"
VALE LO STESSO CHE PER I PIN
*/







void setup(){
 	pinMode(buzzerPin, OUTPUT);
  
	rtc.stopRTC(); //stop the RTC
  rtc.setTime(12,0,0); //set the time here
  rtc.setDate(10,8,2020); //set the date here
  rtc.startRTC(); //start the RTC
  
  pinMode(7, OUTPUT);
  
  Bridge.begin();
  
  mm = millis();
  
  mqtt.begin("test.mosquitto.org", 1883);
  mqtt.subscribe("/tiot/23/sw4/set", decodeCommand);

  registerOnCatalog();

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();

	//ecc ecc
}

void loop(){
	if ((rtc.getHours()==7) && (rtc.getMinutes()==30)){
		senMlEncode("Boiler",1,"");
		senMlEncode("Esterno",0,"");
	}
	if ((rtc.getHours()==8) && (rtc.getMinutes()==0))
		senMlEncode("Window",1,"");
	if ((rtc.getHours()==8) && (rtc.getMinutes()==1))
  		music();
	if ((rtc.getHours()==22) && (rtc.getMinutes()==0)){
		senMlEncode("Esterno",1,"");
		senMlEncode("Boiler",0,"");
		senMlEncode("Window",0,"");
	}
	mqtt.monitor();
  if(millis() - mm >= 20000){
    temp=readTemp();
    senMlEncode("t", temp, "Cel");
    mm = millis();
		setSpeed(temp);
		setLight(temp);
	}
	printLCD();
	
}


void senMlEncode(String res, int v, String unit){
  doc_snd.clear(); //libera memoria utilizzata da doc_snd
  doc_snd["bn"] = "Yun_Camera";
  doc_snd["e"][0]["u"] = unit;
  doc_snd["e"][0]["n"] = res;
  doc_snd["e"][0]["t"] = (long) millis()/1000.0;
  doc_snd["e"][0]["v"] = v;
	String output;
  serializeJson(doc_snd, output);
  mqtt.publish("/tiot/23/sw4/pub", output);
}

void music(){
	int i=0;
	while (i<30){
	tone(buzzerPin, 1000, 500);
  	delay(1000);
	tone(buzzerPin, 1000, 500);
	i++;
  }
}

void setSpeed(float temp){
 	if ((rtc.getMonth()>4) ||(rtc.getMonth()<9)){
    if(temp > fan_min && temp < fan_max) //temperatura in range
      current_speed = v_max * temp / fan_max;
    else if(temp < fan_min) {
      			current_speed = 0;
    }
			else {
      				current_speed = 255;
  				}
    		analogWrite(FAN_PIN, (int) current_speed);
	}
	else analogWrite(FAN_PIN, 0); 
}


void setLight(float temp){
  if ((rtc.getMonth()<4) ||(rtc.getMonth()>9)){
    if(temp > led_min && temp < led_max) { //temperatura in range
        current_light = v_max * (1 - (temp / led_max));
    } else if(temp < led_min){
        current_light = 255;
    } else {
        current_light = 0;
    }
    analogWrite(LED_PIN, (int) current_light);
	}
    else analogWrite(LED_PIN, 0); 
}


void printLCD(){
  String date = ((String) rtc.getHours()) + ":" + ((String) rtc.getMinutes());
  lcd.home();
  lcd.print("T:");
  lcd.print(temp);
  lcd.setCursor(1,0);
  lcd.print("Date:");
  lcd.print(date);
}

float readTemp(){
    int sig = analogRead(TEMP_PIN);
    float R = ((1023.0/sig) - 1.0);
    R = (float) R * R0;
    float log_sig = log(R/R0);
    float temp = 1/((log_sig/B) + 1/298.15) - 273.15;
    return temp;
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

void saveSetpoints(){
  int sl = doc_rcv["e"][0]["sl"];
  int sf = doc_rcv["e"][0]["sf"];
  for (int i = 0; int l = 3; i < 4 && l >= 0, i++, l--) {
    fan_values[l] = sf % 100;
    sf = (int) sf/100;
    led_values[l] = sl % 100;
    sl = (int) sl/100;
  }
  changeSetPoints(); //questa funzione modifica i set points in base alla presenza o meno di una persona
}

void registerOnCatalog(){
  mqtt.publish("/tiot/23/sw4/reg", F("{\"deviceId\":\"Yun_Camera\",\"resorces\":\"lots\",\"end_points\":\"/tiot/23/catalog\"}"));
}

void decodeCommand(const String& topic, const String& subtopic, const String& message){
  	doc_rcv.clear();
  	DeserializationError err = deserializeJson(doc_rcv, message);
  	if(err) {
    		Serial.println(F("deserialize failed with code: "));
    		Serial.println(err.c_str());
  		}
	if(doc_rcv["e"][0]["n"] == "pre"){
		if (doc_rcv["e"][0]["v"] == 0 || doc_rcv["e"][0]["v"] == 1){
			is_present=doc_rcv["e"][0]["n"];
      changeSetPoints();

    }
		else
			Serial.println("Valore errato");
	}
  if(doc_rcv["e"][0]["n"] == "led"){
    if (doc_rcv["e"][0]["v"] == 0 || doc_rcv["e"][0]["v"] == 1) { 
      digitalWrite(7, doc_rcv["e"][0]["v"]); 
    }
		else {
        		Serial.println("Value not correct"); 
    }
  }
  else if(doc_rcv["e"][0]["n"] == "fan"){
    			if (doc_rcv["e"][0]["v"] == 1) 
      				analogWrite(A0, 255); 
    			else if (doc_rcv["e"][0]["v"] == 0)
				analogWrite(A0, 0); 
	//COSTANTE ELIMINATE E VELOCITA' IMPOSTATA COME "ACCESO O SPENTO"   
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
      saveSetpoints();		
		}
}
