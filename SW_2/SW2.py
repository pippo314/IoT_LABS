import cherrypy
import json
import time
from MyMQTT import *



class devices(object):
	def __init__(self, uniqueID, end_points, resources):
		self.uniqueID= uniqueID
		self.end_points= end_points
		self.resources= resources
		self.timestamp_d=time.time()

class users(object):
	def __init__(self):
		self.UserId=""
		self.name=""
		self.surname=""
		self.email=""

class services(object):
	def __init__(self, serviceID, description, end_points):
		self.serviceID= serviceID
		self.description= description
		self.end_points= end_points
		self.timestamp_s = time.time()

class Catalog(object):
	exposed=True

	def __init__(self):
		servi = services("01", "bella", [1, 2])
		self.dev=[]
		self.users=[]
		self.ser=[]
		self.ser.append(servi)
		self.broker = "mqtt.eclipse.org"
		self.port = "8080"

	def searchServices(self, id):
		servizio = [s for s in self.ser if id == s.serviceID]
		data = {"Services": servizio}
		return data

	def searchDevices(self, id):
		dispositivo = [s for s in self.ser if id == s.uniqueID]
		data = {"Dispositivo": dispositivo}
		return data

	def searchUsers(self, id):
		utente = [s for s in self.user if id == s.UserId]
		data = {"Users": utente}
		return data

	# dizionario "Servizi: "ID", "descrizione": "", "end_points":""
	def addServices(self, body):
		for s in self.ser:
			if s.serviceID == body['Servizi']:
				s.timestamp_s = time.time()
				return
		servizio = services(body['Servizi'], body['descrizione'], body['end_points'])
		self.ser.append(servizio)

	# dizionario {"Dispositici: "ID", "risorse": "", "end_points":""}
	def addDevices(self, body):
		for s in self.ser:
			if s.uniqueID == body['Dispositivi']:
				s.timestamp_d = time.time()
				return
		dispositivi = devices(body['Dispositivi'], body['risorse'], body['end_points'])
		self.dev.append(dispositivi)

	# dizionario {"Utenti: "ID", "nome":"", "cognome":"", "email":""}
	def addUsers(self, body):
		utenti = users(body['Utenti'], body['nome'], body['cognome'], body['email'])
		self.users.append(utenti)

	def getServices(self):
		data = {"Services": []}
		n = 0
		for i in self.ser:
			data['Services'] = i.serviceID
			n += 1
		return data

	def getUsers(self):
		data = {"Users": []}
		n = 0
		for i in self.users:
			data['Users'][n] = i.UserId
			n += 1
		return data

	def getDevices(self):
		data = {"Devices": []}
		n = 0
		for i in self.dev:
			data['Devices'][n] = i.uniqueID
			n += 1
		return data

	def getIp(self):
		data = {"Ip": self.broker, "Porta": self.port}
		return data

	#tutto come uri
	def GET(self, *uri, **params):
		if uri[0]=="services":
			if uri[1]=="list":
				return self.getServices()
			elif uri[1]=="search":
				return self.searchServices(uri[2])
			else:
				return "Comando non valido"
		elif uri[0]=="users":
			if uri[1]=="list":
				return self.getUsers()
			elif uri[1]=="search":
				return self.searchUsers(uri[2])
			else:
				return "Comando non valido"
		elif uri[0]=="devices":
			if uri[1]=="list":
				return self.getDevices()
			elif uri[1]=="search":
				return self.searchDevices(uri[2])
			else:
				return "Comando non valido"
		elif uri[0]=="ip":
			return self.getIp()
		else:
			return "Comando no supportato, riprova"


		#*uri restituisce una lista di valori, **params restituisce un dizionario
	#def POST(self, *uri, **params):
		#do something
		

	
		
		#return some_string

	def PUT (self, *uri, **params):
		body = cherrypy.request.body.read()
		json_body = json.loads(body.decode('utf-8'))
		if uri[0]=="services":
			self.addServices(json_body)
		elif uri[0]=="users":
			self.addUsers(json_body)
		elif uri[0]=="devices":
			self.addDevices(json_body)
		else:
			pass

	#def DELETE(self):




if __name__=='__main__':
	conf = {
		'/': {
			'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
			'tool.session.on': True
		}
	}
	cherrypy.tree.mount(Catalog(),'/',conf)
	#cherrypy.config.update({'server_socket_host':'0.0.0.0'})
	#cherrypy.config.update({'server_socket_port':8080})
	cherrypy.engine.start()
	cherrypy.engine.block()
