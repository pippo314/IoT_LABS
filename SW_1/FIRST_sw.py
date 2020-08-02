import json
import cherrypy
import pytemp
import array

keys={"K","C","F"}

def controllo_chiavi(chiavi):
	if len(chiavi)!=3:
		if chiavi[0]=="value" or chiavi[1]=="originalUnit" or chiavi[2]=="targetUnit":
			return 0
	else:
		return 1
	


def trasforma_valori(valori):
	if len(valori)!=3:
		if str.isdigit(valori[0]) or keys.intersection(valori[1]) or keys.intersection(valori[2]):
			return None
	i=0
	c=[3,1,2]
	for p in valori:
		c[i]=str(p)
		i+=1
	if c[1]==c[2]:
		return None
	if c[1]=="k" or c[1]=="K":
		if int(c[0]) < 0:
			return None
	x=pytemp.pytemp(int(c[0]),c[1],c[2])
	return x




class MyWebServices(object):

	exposed=True

	def GET(self,*uri,**params):
		r="".join(uri)
		if uri!="":
			if r=="converter":
				if params!="":
					k=params.keys()
					v=params.values()
					x=controllo_chiavi(k)
					if x==0:
						return "errore"
					x=trasforma_valori(v)
					if x==None:
						return "errore"
					else:
						c=[1,2,3]
						i=0
						for p in v:
							c[i]=str(p)
							i+=1
						diz={str(c[0]) : str(c[1]), str(x) : str(c[2])}
						pp=json.dumps(diz)
						return pp
			else:
				return "funzione non richiamata correttamente"

