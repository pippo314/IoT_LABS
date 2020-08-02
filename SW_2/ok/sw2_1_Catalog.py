import cherrypy
import json
import time
from sw2_1_Collector import *

""" FORMATO JSON:
	Device: { 
		"deviceId": "",
		"resources": "",
		"end_points": [],
		"timestamp": 0
	}

	User: {
		"userId": "",
		"name": "",
		"surname": "",
		"email": "",
		"timestamp": 0
	}

	Service: {
		"serviceId": "",
		"description": "",
		"end_points": [],
		"timestamp": 0
	}
	"""


class Devices:
	""" classe che espone le funzioni di GET E PUT
		per i dispositivi del Catalog """

	exposed=True

	def __init__(self):
		self.devices = Collector()

	def GET(self, *uri, **params):
		""" list ritorna un json nel formato {"List": []}
			search ritorna un json con le info del Device """

		if uri[0]=="list":
			return self.devices.listItems()
		elif uri[0]=="search":
			try:
				return self.devices.search(uri[1])
			except KeyError:
				raise cherrypy.HTTPError(404, "Device not found.")
		else:
			raise cherrypy.HTTPError(400, "Bad request, try again.")

	def PUT (self, *uri, **params):
		""" add salva nel Collector della classe l'oggetto Device
			creato con le info ricevute come json nel body 
			oppure aggiorna il timestamp se il device è già
			presente nel Catalog """
			

		body = cherrypy.request.body.read().decode('utf-8')
		if body == '':
			raise cherrypy.HTTPError(400, "Bad request, empty body")
		json_body = json.loads(body)
		if uri[0] == "add":
			deviceId = json_body["deviceId"]
			if self.devices.contains(deviceId):
				self.devices.updateItemTimestamp(deviceId)
			else:
				resources = json_body["resources"]
				end_points = json_body["end_points"]
				d = Device(deviceId, end_points, resources)
				self.devices.add(d)

class Users:
	""" classe che espone GET e PUT per gli utenti del Catalog """

	exposed=True

	def __init__(self):
		self.users = Collector()

	def GET(self, *uri, **params):
		""" list ritorna una lista degli utenti come json
			nel formato {"List" : []}
			search ritorna un json con le info dell'utente """

		if uri[0]=="list":
			return self.users.listItems()
		elif uri[0]=="search":
			try:
				return self.users.search(uri[1])
			except KeyError:
				raise cherrypy.HTTPError(404, "User not found.")
		else:
			raise cherrypy.HTTPError(400, "Bad request, try again.")

	def PUT (self, *uri, **params):
		""" add salva nel Collector l'oggetto User creato 
			con le info ricevute tramite json nel body o
			aggiorna il timestamp se l'utente è già presente """

		body = cherrypy.request.body.read().decode('utf-8')
		if body == '':
			raise cherrypy.HTTPError(400, "Bad request, empty body")
		json_body = json.loads(body)
		if uri[0] == "add":
			userId = json_body["userId"]
			if self.users.contains(userId):
				self.users.updateItemTimestamp(userId)
			else:
				name = json_body["name"]
				surname = json_body["surname"]
				email = json_body["email"]
				u = User(userId, name, surname, email)
				self.devices.add(u)

class Services:
	""" classe che espone GET e PUT per i servizi nel Catalog """

	exposed=True

	def __init__(self):
		self.services= Collector()

	def GET(self, *uri, **params):
		""" list ritorna una lista di tutti i servizi come json
			nel formato {"List": []}
			search ritorna le info del servizio tramite json """
		if uri[0]=="list":
			return self.services.listItems()
		elif uri[0]=="search":
			try:
				return self.services.search(uri[1])
			except KeyError:
				raise cherrypy.HTTPError(404, "Service not found.")
		else:
			raise cherrypy.HTTPError(400, "Bad request, try again.")

	def PUT (self, *uri, **params):
		""" add aggiunge un servizio al Catalog con le info
			ricevute tramite json nel body o se il servizio
			è già presente aggiorna il suo timestamp """

		body = cherrypy.request.body.read().decode('utf-8')
		if body == '':
			raise cherrypy.HTTPError(400, "Bad request, empty body")
		json_body = json.loads(body)
		if uri[0] == "add":
			serviceId = json_body["serviceId"]
			if self.services.contains(serviceId):
				self.services.updateItemTimestamp(serviceId)
			else:
				description = json_body["description"]
				end_points = json_body["end_points"]
				s = Service(serviceId, description, end_points)
				self.devices.add(s)

class Broker:
	""" classe web che espone una GET per richiedere le informazioni
		sul message broker e la porta utilizzati dai dispositivi 
		all'interno del Catalog """

	exposed=True
	
	def __init__(self):
		self.broker = "test.mosquitto.org"
		self.port = 1883

	def GET(self, *uri, **params):
		""" ritorna tramite json il message broker e la porta utilizzati """
		if uri[0]=="info":
			return json.dumps({
				"brokerIp": self.broker,
				"brokerPort": self.port
				})
		else:
			raise cherrypy.HTTPError(400, "Bad request, try again.")