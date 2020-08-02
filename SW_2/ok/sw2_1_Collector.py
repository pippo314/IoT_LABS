import time
from sw2_1_Classi import *

class Collector:
    """ classe contenitore pdi oggetti con funzionalità
        base per una collezione di dati

        items: dizionario in forma {"Id": Oggetto}

        add(): aggiunge un oggetto o aggiorna il timestamp
        search():  ritorna l'oggetto come json o genera KeyError
        listItems(): ritorna un json con la lista di tutti gli elementi
    """

    def __init__(self):
        self.items = {}

    def add(self, item):
        if item.Id in self.items:
            self.items[item.Id].updateTimestamp()
        else:
            self.items[item.Id] = item

    def search(self, Id):
        return json.dumps(self.items[Id].asJSON())
        
    def contains(self, Id):
        return Id in self.items.keys()
        
    def updateItemTimestamp(self, Id):
        self.items[Id].updateTimestamp()

    def listItems(self):
        l = []
        for i in self.items.values():
            l.append(i.toJSON())
        return json.dumps({"List": l})