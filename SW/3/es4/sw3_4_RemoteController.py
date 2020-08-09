import requests
import json
import cherrypy
from sw3_4_MyMQTT import *

class RemoteController():

	exposed = True

	def __init__(self):
		self.mqtt_client = MyMQTT("MyCkient_1", "test.mosquitto.org", 1883)
		self.mqtt_client.start()

	def GET(self, *uri, **params):
		# http://localhost:8080/controller/?command=temp

		if "Command" in params.keys():
			command = params["Command"]
			if command == "PRESENCE":
				ret = self.mqtt_client.msg["Presence"]
				return json.dumps(ret)
			elif command == "TEMP":
				ret = self.mqtt_client.msg["Temperature"]
				return json.dumps(ret)
			elif command == "NOISE":
				ret = self.mqtt_client.msg["Noise"]
				return json.dumps(ret)
			else:		
				raise cherrypy.HTTPError(400, "Bad request, wrong command.")
		else:		
			raise cherrypy.HTTPError(400, "Bad request, wrong or missing command.")

	def PUT(self, *uri, **params):
		body = cherrypy.request.body.read().decode('utf-8')		
		if body == '':
			raise cherrypy.HTTPError(400, "Bad request, empty body")
		json_body = json.loads(body)
		command = json_body["Command"]
		msg = {
			"bn": "Yun", 
			"e": [
				{
				"n": "", 
				"t": "null", 
				"v": 0, 
				"u": "null",
				"s": ""
					}
				]
			}
		if command == "FAN_ON":
			msg["e"][0]["n"] = "fan"
			msg["e"][0]["v"] = 1
			self.mqtt_client.myPublish(msg)
		elif command == "FAN_OFF":
			msg["e"][0]["n"] = "fan"
			msg["e"][0]["v"] = 0
			self.mqtt_client.myPublish(msg)
		elif command == "LED_ON":
			msg["e"][0]["n"] = "led"
			msg["e"][0]["v"] = 1
			self.mqtt_client.myPublish(msg)
		elif command == "LED_OFF":
			msg["e"][0]["n"] = "led"
			msg["e"][0]["v"] = 0
			self.mqtt_client.myPublish(msg)
		elif command == "CHANGE_SETPOINTS":
			msg["e"][0]["n"] = "setpoints"
			msg["e"][0]["s"] = json_body["values"]
			self.mqtt_client.myPublish(msg)
		elif command == "PRINT_LCD":
			msg["e"][0]["n"] = "print"
			msg["e"][0]["s"] = json_body["values"]
			self.mqtt_client.myPublish(msg)
		else:
			raise cherrypy.HTTPError(400, "Bad request, wrong command.")
