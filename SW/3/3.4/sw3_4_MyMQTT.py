import json
import requests
import paho.mqtt.client as mqtt

class MyMQTT:
	def __init__(self, clientID, broker, port):
		self.broker = broker
		self.port = port
#		self.notifier = notifier 	
		self.clientID = clientID

		self._topic = ""
		self._isSubscriber = False

		self.msg = { 
			"Presence": {"Errore": "Messaggio vuoto."}, 
			"Noise": {"Errore": "Messaggio vuoto."}, 
			"Temperature": {"Errore": "Messaggio vuoto." }
		}

		self.serviceId ="" 
		self.description ="" 
		self.end_points = ""

		

		# create an instance of paho.mqtt.client
		self.mqtt_client = mqtt.Client(clientID, False) 

		# register the callback
		self.mqtt_client.on_connect = self.myOnConnect
		self.mqtt_client.on_message = self.myOnMessageReceived
		
		self.registerOnCatalog()
		self.getBrokerInfo()
		self.getDeviceTopic("Yun")
		


	def myOnConnect (self, paho_mqtt, userdata, flags, rc):
		print ("Connected to %s with result code: %d" % (self.broker, rc))

	def myOnMessageReceived (self, paho_mqtt , userdata, msg):
		print("message rcv")
		body = msg.payload.decode('utf-8') 
		print(body)
		json_msg = json.loads(body)

		if json_msg["e"][0]["n"] == "t":
			self.msg["Temperature"] = json_msg
		elif json_msg["e"][0]["n"] == "n": 
			self.msg["Noise"] = json_msg
		elif json_msg["e"][0]["n"] == "p":
			self.msg["Presence"] = json_msg
		# A new message is received
		print ("Topic:'" + msg.topic+"', QoS: '"+str(msg.qos)+"' Message: '"+str(msg.payload) + "'")
#		self.notifier.notify (msg.topic, msg.payload)


	def myPublish (self, msg):
		# if needed, you can do some computation or error-check before publishing
		print ("publishing '%s' with topic '%s'" % (msg, self._topic))
		
		# publish a message with a certain topic
		str_msg = json.dumps(msg)
		self.mqtt_client.publish(self._topic, str_msg, 2)

	def mySubscribe (self, topic):
		# if needed, you can do some computation or error-check before subscribing
		print ("subscribing to %s" % (topic))
		# subscribe for a topic
		self.mqtt_client.subscribe(topic, 2)

		# just to remember that it works also as a subscriber
		self._isSubscriber = True

	def start(self):
		#manage connection to broker
		self.mqtt_client.connect(self.broker , self.port)
		self.mqtt_client.loop_start()
		for t in self._topic:
			self.mySubscribe(t)

	def stop (self):
		if (self._isSubscriber):
			# remember to unsuscribe if it is working also as subscriber 
			self.mqtt_client.unsubscribe(self._topic)

		self.mqtt_client.loop_stop()
		self.mqtt_client.disconnect()

	def registerOnCatalog(self):
		service = {
			"serviceId": self.serviceId,
			"description": self.description,
			"end_points": self.end_points
		}
		requests.put("http://192.168.1.52:8080/services/add", json = service)

	def getBrokerInfo(self):
		r = requests.get("http://192.168.1.52:8080/broker/info") #controllare che cosa ritorna requests
		info = json.loads(r.content.decode('utf-8'))
		self.messageBroker = info["brokerIp"]
		self.port = info["brokerPort"]
#		print(f"{self.port} {self.messageBroker}")

	def getDeviceTopic(self, deviceId):
		r = requests.get("http://192.168.1.52:8080/devices/search/"+deviceId)
		c = r.content.decode('utf-8')
		if c == '':
			print("Device non trovato.")
		else:
			info = json.loads(r.content.decode('utf-8'))
			self._topic = info["end_points"]
		print(self._topic)	
#		for t in self._topic:
#			self.mySubscribe(t)
#		print(self._topic)

#		{ "end_points": {"sub": ["topic"], "pub": ["altri topic"]}}

