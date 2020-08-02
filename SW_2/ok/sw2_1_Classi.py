import json
import time


class Device:
	""" Rappresenta il device all'interno del Catalog

		FORMATO JSON:
		Device: { 
			"deviceId": "",
			"resources": "",
			"end_points": [],
			"timestamp": 0
		}
		
		Id: identificativo unico
		end_points: servizi cui è collegato il dispositivo
		resources: risorse messe a disposizione dal dispositivo
		timestamp: ora di aggiunta del dispositivo al catalog

		asJSON(): ritorna l'oggetto come json
		updateTimestamp(): aggiorna il timestamp
		"""

	def __init__(self, ID, end_points, resources):
		self.Id = ID
		self.end_points = end_points
		self.resources = resources
		self.timestamp = int(time.time()/60)

	def asJSON(self):
		return {
			"deviceId": self.Id,
			"resources": self.resources,
			"end_points": self.end_points,
			"timestamp": self.timestamp
		}
	def updateTimestamp(self):
		self.timestamp = int(time.time()/60)


class User:
	""" Rappresenta l'utente nel Catalog

		FORMATO JSON:
		User: {
			"userId": "",
			"name": "",
			"surname": "",
			"email": "",
			"timestamp": 0
		}
        
        Id: identificatore dell'utente
        name: nome utente
        surname: cognome utente
        email: email utente

		asJSON(): ritorna l'oggetto come json
		updateTimestamp(): aggiorna il timestamp
    """

	def __init__(self, ID, name, surname, email):
		self.Id=ID
		self.name=name
		self.surname=surname
		self.email=email
		self.timestamp = int(time.time()/60)

	def asJSON(self):
		return {
			"userId": self.Id,
			"Name": self.name,
			"Surname": self.surname,
			"email": self.email,
			"timestamp": self.timestamp
		}

	def updateTimestamp(self):
		self.timestamp = int(time.time()/60)
		
class Service:
	""" Rappresenta un servizio presente nel Catalog

		FORMATO JSON:
		Service: {
			"serviceId": "",
			"description": "",
			"end_points": [],
			"timestamp": 0
		}

        Id: identificatore del servizio
        description: breve descrizione delle funzionalità del servizio
        end_points: servizi cui è collegato il servizio
        timestamp: ora di aggiunta del servizio al catalog

		asJSON(): ritorna l'oggetto come json
		updateTimestamp(): aggiorna il timestamp
    """
	
	def __init__(self, serviceID, description, end_points):
		self.Id= serviceID
		self.description= description
		self.end_points= end_points
		self.timestamp = int(time.time()/60)

	def asJSON(self):
		return {
			"serviceId": self.Id,
			"description": self.name,
			"end_points": self.surname,
			"timestamp": self.timestamp
		}
	def updateTimestamp(self):
		self.timestamp = int(time.time()/60)