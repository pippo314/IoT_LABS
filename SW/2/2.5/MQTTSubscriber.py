import paho.mqtt.client as mqtt
import time
import json


class MQTTSubscriber:

    def __init__(self, clientID, broker, port, topic, notifier):
        self.clientID = clientID
        self.topic = topic
        self.broker =broker
        self.notifier = notifier
        self.port=port
        self.mqtt_client = mqtt.Client(clientID, False)
        self.mqtt_client.on_connect = self.myOnConnect
        self.mqtt_client.on_message = self.myOnMessageReceived

    def start(self):
        self.mqtt_client.connect(self.broker, self.port)
        self.mqtt_client.loop_start()
        self.mqtt_client.subscribe(self.topic, 2)

    def stop(self):
        self.mqtt_client.unsubscribe(self.topic)
        self.mqtt_client.loop_stop()
        self.mqtt_client.disconnect()

    def myOnConnect(self, paho_mqtt, userdata, flags, rc):
        print("Connected to %s with result code: %d" % (self.broker, rc))

    def myOnMessageReceived(self, paho_mqtt, userdata, msg):
        data = msg.payload.decode('utf-8')
        my_json = json.loads(data)
        self.notifier.registerFromMQTT(my_json)
        # risposta del tipo {"Device":"Id", "resources":[], "end_points":[]}
