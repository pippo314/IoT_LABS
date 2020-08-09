import paho.mqtt.client as mqtt
import time
import json


class MQTTSubscriber:

    def __init__(self, clientID, broker, port, topic, notifier):
        self.clientID = clientID
        self.topic = topic #/tiot/23/sw4/reg --- /tiot/23/sw4/pub 
        self.broker =broker
        self.notifier = notifier
        self.port=port
		self.msg = { 
			"Presence": {"Errore": "Messaggio vuoto."}, 
			"Noise": {"Errore": "Messaggio vuoto."}, 
			"Temperature": {"Errore": "Messaggio vuoto." }
		}
        self.mqtt_client = mqtt.Client(clientID, False)
        self.mqtt_client.on_connect = self.myOnConnect
        self.mqtt_client.on_message = self.myOnMessageReceived

		self.registerOnCatalog()
		self.getBrokerInfo()

    def start(self):
        self.mqtt_client.connect(self.broker, self.port)
        self.mqtt_client.loop_start()
        for t in self.topic:
            self.mqtt_client.subscribe(t, 2)

    def stop(self):
        self.mqtt_client.unsubscribe(self.topic)
        self.mqtt_client.loop_stop()
        self.mqtt_client.disconnect()

    def myOnConnect(self, paho_mqtt, userdata, flags, rc):
        print("Connected to %s with result code: %d" % (self.broker, rc))

	def registerOnCatalog(self):
		service = {
			"serviceId": self.clientId,
			"description": "service",
			"end_points": self.topic
		}
		requests.put("http://localhost:8080/services/add", json = service)

	def getBrokerInfo(self):
		r = requests.get("http://localhost:8080/broker/info") #controllare che cosa ritorna requests
		info = json.loads(r.content.decode('utf-8'))
		self.messageBroker = info["brokerIp"]
		self.port = info["brokerPort"]

    def myOnMessageReceived(self, paho_mqtt, userdata, msg):
        data = msg.payload.decode('utf-8')
        my_json = json.loads(data)
        if "deviceId" in my_json.keys():
            self.notifier.registerFromMQTT(my_json)
        elif "bn" in my_json.keys():
            if my_json["n"] == "Boiler":
                self.mqtt_client.publish("/tiot/boiler", {"value": my_json["v"]}, 2)
            elif my_json["n"] == "Window":
                self.mqtt_client.publish("/tiot/window", {"value": my_json["v"]}, 2)
            elif my_json["n"] == "Esterno":
                self.mqtt_client.publish("/tiot/23/sw4/pir", {"value": my_json["v"]}, 2)
            elif json_msg["e"][0]["n"] == "t":
                self.msg["Temperature"] = json_msg
            elif json_msg["e"][0]["n"] == "n": 
                self.msg["Noise"] = json_msg
            elif json_msg["e"][0]["n"] == "p":
                self.msg["Presence"] = json_msg
            elif json_msg["e"][0]["n"] == "Password Errata.":
                print("Password errata!")
            elif json_msg["e"][0]["n"] == "Allarmi Intrusi.":
                print("Allarme Intrusi!")

        # risposta del tipo {"Device":"Id", "resources":[], "end_points":[]}
