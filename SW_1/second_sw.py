import json
import cherrypy
import pytemp
import array

keys={"K","C","F"}
"""
def controllo_chiavi(chiavi):
	if len(chiavi)!=3:
		if chiavi[0]=="value" or chiavi[1]=="originalUnit" or chiavi[2]=="targetUnit":
			return 0
	else:
		return 1
	"""





def check_uri(uri):
    if len(uri)==4:
    	if uri[0]=='converter' or str.isdigit(uri[1] or uri[2] in keys or uri[3] in keys):
       		check_temp(uri[1], uri[2], uri[3])
        	return True
    return False

def check_temp(temp, unit1, unit2):
    if unit1 == unit2:
        return False
    else:
        return True

def convert(uri):
    return pytemp.pytemp(int(uri[1]), uri[2], uri[3])






class MyWebServices(object):

	exposed=True

	def GET(self,*uri):
		if uri != '':
			
			ind = ' '.join(uri)
			
			ind = ind.split(' ')

			if check_uri(ind):

				ind.append(convert(ind))
				i=1
				diz = {'value':'',
                   'originalUnit':'',
                   'targetUnit':'',
                   'valueConvert':''
           		}

				for key in diz.keys():
					diz[key] = ind[i]
					i+=1

				return json.dumps(diz)
			else:
				return "Valori passati non corretti"
			
            
		else:
			return "Nessuna richiesta"



		"""r="".join(uri)
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
				return "funzione non richiamata correttamente" """

