


class devices():
	def __init__(self, uniqueID, end_points, resources):
		self.Id= uniqueID
		self.end_points= end_points
		self.resources= resources
		self.timestamp=time.time()

# classe users con attributi corrispondenti
class users():
	def __init__(self,ID, name,surname,email):
		self.Id=ID
		self.name=name
		self.surname=surname
		self.email=email
		self.timestamp = time.time()

# classe services con attributi corrispondenti
class services():
	def __init__(self, serviceID, description, end_points):
		self.Id= serviceID
		self.description= description
		self.end_points= end_points
		self.timestamp = time.time()


class funzioni_varie():
# METODO  PER LA RICERCA DI UN ELEMENTO ALL'INTERNO DI UNO DEI VARI SERVIZI
	def search(list[],id,stri):
		i=[s for s in self.list if id == s.Id]
		if i==None:
			return"non trovato"
		else: 
			data={stri : +i}
			return data

#METODO CHE RITORNA L'INTERNA LISTA DI ELEMENTI
	def get(lis[],stri):
		data = {stri : []}
		n = 0
		for i in lis:
			data[stri][n] = i.Id
			n += 1
		return data

#METODO CHE AGGIUNGE UN ELEMENTO
	def add(lis[],body,stri)
		for s in lis:
			if s.Id==body[stri]:
				s.timestamp=time.time()
				return "Dati gia' presenti"
		x=None
		if stri=="Servizi":
			x=services(body['Servizi'], body['descrizione'], body['end_points'])
			lis.append(x)
		elif stri=="Devices":
			x=devices(body["dispositivi"],body['risorse'],body['end_points'])
			lis.append(x)
		elif stri=="Users":
			x=users(body['Utenti'], body['nome'], body['cognome'], body['email'])
			lis.append(x)
		return x

#METODO CHE AGGIORNA IL TIMESTAMP
		def updateTimestamp(lis[], num):
		if lis[num]!=None:
			lis[num].timestamp = time.time()


#METODO CHE RITORNA LA PORTA ED IL MESSAGE BROKER
	def getIp(brok,port):
		data = {"Ip": brok, "Porta": port}
		return data
