import cherrypy
import json
import time
from CLASSI_nonweb_sw2_1 import*


# classe device con attributi corrispondenti
class dev():
	exposed=True
	def __init__(self):
		disp=[]

	def GET(self, *uri, **params):
		if uri[0]=="list":
			return funzioni_varie.get(self.disp,"Device")
		elif uri[0]=="search":
			return funzioni_varie.search(self.disp,uri[1],"dispositivi")
		else:
			return "Comando non valido"

	def PUT (self, *uri, **params):
		body = cherrypy.request.body.read()
		json_body = json.loads(body.decode('utf-8'))
		if uri[0] == "devices":
			t=funzioni_varie.add(self.disp,body,"Devices")
			if t==None:
				return "Errore"
			else:
				return "TUTTO OK"
		else:
			return "Nessun campo nel body della funzione PUT"


# classe users con attributi corrispondenti
class us():
	exposed=True
	def __init__(self):
		us=[]

	def GET(self, *uri, **params):
		if uri[0]=="list":
			return funzioni_varie.get(self.us,"Users")
		elif uri[0]=="search":
			return funzioni_varie.search(self.us,uri[1],"Users")
		else:
			return "Comando non valido"

	def PUT (self, *uri, **params):
		body = cherrypy.request.body.read()
		json_body = json.loads(body.decode('utf-8'))
		if uri[0] == "users":
			t=funzioni_varie.add(self.us,body,"Users")
			if t==None:
				return "Errore"
			else:
				return "TUTTO OK"
		else:
			return "Nessun campo nel body della funzione PUT"

# classe services con attributi corrispondenti
class ser():
	exposed=True
	def __init__(self):
		serv=[]

	def GET(self, *uri, **params):
			if uri[0]=="list":
				return funzioni_varie.get(self.disp,"Services")
			elif uri[0]=="search":
				return funzioni_varie.search(self.serv,uri[1],"Services")
			else:
				return "Comando non valido"

	def PUT (self, *uri, **params):
		body = cherrypy.request.body.read()
		json_body = json.loads(body.decode('utf-8'))
		if uri[0] == "services":
			t=funzioni_varie.add(self.serv,body,"Servizi")
			if t==None:
				return "Errore"
			else:
				return "TUTTO OK"
		else:
			return "Nessun campo nel body della funzione PUT"

class br(object):
	exposed=True
	
	def __init__(self):
		self.broker = "mqtt.eclipse.org"
		self.port = 8080

	

	def GET(self, *uri, **params):
		if uri[0]=="ip":
			return funzioni_varie.getIp(self.broker,self.port)
		else:
			return "Comando non supportato, riprova"




	
	

	

	
	

	


	#file json atteso del tipo
	# {"Servizi: "ID", "descrizione": "", "end_points": [""]}

	# metodo per l'aggiunta di un nuovo servizio. Controllo sull'aggiunta di un servizio dublicato
	# e corrispondente aggiornamento del timestamp
	

	#file json atteso del tipo
	# {"Dispositici: "ID", "risorse": [""], "end_points":[""]}

	

	#file json atteso del tipo
	# {"Utenti: "ID", "nome":"", "cognome":"", "email":""}

	# metodo per l'aggiunta di un nuovo utente. 
	

	# meto che ritorna un dizionario composto da {"Services": ["lista di servizi con il proprio serviceID"]}
	


	


	
	


	