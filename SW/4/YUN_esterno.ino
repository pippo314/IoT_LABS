#include <Keypad.h> // by Mark Stanley
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <Process.h>
#include <Bridge.h>
#include <ArduinoJson.h>
#include <MQTTclient.h>

//tastierino
const byte ROWS = 4;
const byte COLS = 4;
const int n_pass = 4;
int pass_insert = 0;
int pass[4];

DynamicJsonDocument doc_snd(JSON_OBJECT_SIZE(3)+ JSON_ARRAY_SIZE(1)+ JSON_OBJECT_SIZE(8));
DynamicJsonDocument doc_rcv(JSON_OBJECT_SIZE(8));

char keys[ROWS][COLS]={
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
  	{'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {5, 6, 8, 9}; 
byte colPins[ROWS] = {10, 11, 12, 13};
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int value = LOW;

//pin sensori
const int SOUND_PIN = 4;
const int PIR_PIN_EST = 7;
const int PIR_PIN_INT = 1;
const int LIGHT_PIN = 3;
const int LED_EST_PIN = 2;
 
//variabili presenza
int pir_is_present = 0;
int sound_is_present = 0;
int is_present = 0;

//variabili sensore suono
unsigned long int sound_event;
const int timeout_sound = 60000;
unsigned long int ss;
const int minutes5 = 300000;
boolean first_half = true;
int n_events_first = 0;
int n_events_second = 0;
unsigned long int camp = 0;
const int sound_interval = 500;
unsigned long int s = 0;
int n_sound_events;



//variabili sensore movimento
unsigned long int pir_t; 
const int pir_tt=1000;
boolean past_22 = false;

//variabili lampadina smart
unsigned long int c;
const int light_time = 500;

//variabili generali
unsigned long int mm; //per timer di invio valori sensori

void encodeAndSend(String res, int v, String unit){
  doc_snd.clear(); //libera memoria utilizzata da doc_snd
  doc_snd["bn"] = "Yun_Esterno";
  doc_snd["e"][0]["u"] = unit;
  doc_snd["e"][0]["n"] = res;
  doc_snd["e"][0]["t"] = (long) millis()/1000.0;
  doc_snd["e"][0]["v"] = v;
  String output;
  serializeJson(doc_snd, output);
  Serial.println(output);
  mqtt.publish("/tiot/23/sw4/pub", output);
}

//controllo sulla password immessa 
int input_check_key(){
	char c;
	int pass_tmp[4], flag;
	int i=(flag=0);
	while((c = getKey() )!=NO_KEY && !flag && i<5){
		if(isdigit(c)){
			if((int) pass_tmp[i] != pass[i]){
				flag=1;
			}
			i++;
		}else{
			if(c != 'D' && i==4){
				//B carattere di terminazione
				//carattere errato 
			}
			flag=1;
		}
	}
	if(flag){
		//errore nell'inserimento della password
		//bisogna inviare al server messaggio di tentato accesso
		encodeAndSend(F("Password Errata."), 0, "null"); //deve richiamare una funzione che invia un messaggio tramite mqtt al server
		return 0; 
	}else{
		//accesso riuscito, posso andare a cambiare il valore di pass_insert
		return 1;
	}
}

//inizializzazione della password
int setPassword(){
	char c; 
	int i, flag=0;
	while((c=getKey() ) == "B" && !flag){
		//sono pronto ad iniziare a leggere la password
		i=0;
		while((c=getKey() )!=NO_KEY && i<5){
			if(isdigit(c)){
				pass[i]=(int) c;
				i++;
			}else{
				if(c != 'D' && i==4){
					//B carattere di terminazione
					//carattere errato 
				}
				flag=1;
				break;
			}
		}
	}
}

//sensore di rumore per la luce che funziona con il battito di mani
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

//sensore di movimento esterno per luce
void PIRisr(){
  if(past_22){
	digitalWrite(LED_EST_PIN, HIGH);
	delay(5000);
	digitalWrite(LED_EST_PIN, LOW);
  }
}


//sensore di rumore interno 
void readSound(){
  if(millis() - sound_event >= timeout_sound){ //60 minuti set is_present
    sound_event = millis();
    sound_is_present = 0;
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

void event(){
  if(first_half == true){
    n_events_first++;
  } else n_events_second++;
}

void check_presence(){
	is_present = sound_is_present | pir_is_present;
	if(pass_insert && is_present){
		//sistema di sicurezza attivo e rilevata presenza
		pir_is_present = 0; //azzero la variabile di controllo del pir
		encodeAndSend("Allarme intrusi.", 0, "null");
	}
}

//se viene rilevato movimento è inutile andare a contare quanti movimenti ci sono stati 
//sensore di movimento interno per il sistema di sicurezza
void readMove(){
	if(millis()-pir_t>pir_tt){
		if(digitalRead(PIR_PIN_INT))
			pir_is_present=1;
	}
}

void registerOnCatalog(){
  mqtt.publish("/tiot/23/sw4/reg", F("{\"Device\":\"Yun_Esterno\",\"Resorces\":\"lots\",\"end_points\":\"/tiot/23/catalog\"}"));
}

void decodeCommand(const String& topic, const String& subtopic, const String& message){
    doc_rcv.clear();
    DeserializationError err = deserializeJson(doc_rcv, message);
    if(err) {
        Serial.println(F("deserialize failed with code: "));
        Serial.println(err.c_str());
      }
  if(doc_rcv["value"] == 1){
    past_22 = true;
  } else if(doc_rcv["value"] == 0){
    past_22 = false;
  }
}

void setup(){
	Serial.begin(9600);
  Bridge.begin();

  attachInterrupt(digitalPinToInterrupt(PIR_PIN_EST), PIRisr, FALLING);

 	pinMode(SOUND_PIN, INPUT);
	pinMode(PIR_PIN_EST, INPUT);
	pinMode(PIR_PIN_INT, INPUT);
	pinMode(LIGHT_PIN, OUTPUT);
	pinMode(LED_EST_PIN, OUTPUT);

 	
 	mqtt.begin("test.mosquitto.org", 1883);
	mqtt.subscribe("/tiot/23/sw4/pir", decodeCommand);
  registerOnCatalog();

	setPassword();
  
  camp = millis();
  s = millis();
  pir_t = millis();
  mm = millis();
  ss = millis();
  sound_event = millis();
}

void loop(){
	if(pass_insert){
		//devo inserire la password, memorizzo la chiave inserita, controllo che sia giusta 
		if(input_check_key()){
			pass_insert = 0;
		}
	}else{
		//stessa cosa di su 
		if(input_check_key()){
			pass_insert = 1;
		}
	} 
    //invio valori al controller remoto
  if(millis() - mm >= 20000){
     encodeAndSend("n", sound_event, "null");
      encodeAndSend("p", is_present, "null");
      mm = millis();
  }
	readSound();
	readMove();
	check_presence();
	turnLightOn();
}
