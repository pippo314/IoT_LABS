import requests
import json

# dizionario comune per le richieste al server
body = {
      "Command":"null",
      "values": "1 2 3 4"
    }

def resetDict(body):
  body = {
        "Command":"null",
        "values": ""
      }

def fanOn(address):
  body["Command"] = "FAN_ON"
  requests.put(address, json = body)
  resetDict(body)

def fanOff(address):
  body["Command"] = "FAN_OFF"
  requests.put(address, json = body)
  resetDict(body)

def ledOn(address):
  body["Command"] = "LED_ON"
  requests.put(address, json = body)
  resetDict(body)

def ledOff(address):
  body["Command"] = "LED_OFF"
  requests.put(address, json = body)
  resetDict(body)

def presence(address):
  #http://localhost:8080/controller/?Command=example
  r = requests.get(address + "/?Command=PRESENCE")
  s = r.content.decode('utf-8')
  print(f"Il contenuto è:{s}")
  resetDict(body)

def noise(address):
  r = requests.get(address + "/?Command=NOISE")
  s = r.content.decode('utf-8')
  print(f"Il contenuto è:{s}")
  resetDict(body)

def temp(address):
  r = requests.get(address + "/?Command=TEMP")
  s = r.content.decode('utf-8')
  print(f"Il contenuto è:{s}")
  resetDict(body)

def changeSetpoints(address):
  body["Command"] = "CHANGE_SETPOINTS"
  setpoints = input("Inserire i 4 setpoints: ")
  body["values"] = setpoints
  requests.put(address, json = body)
  resetDict(body)

def printLCD(address):
  body["Command"] = "PRINT_LCD"
  s = input("Inserire il messaggio da stampare sullo schermo LCD: ")
  body["values"] = s
  requests.put(address, json = body)
  resetDict(body)

def implementationMenu(address):
  while True:
    command = input(
      "Inserire comando:\nFAN_ON Accendere condizionatore\nFAN_OFF Spegnere condizionatore\nLED_ON Accendere riscaldamento\nLED_OFF Spegnere riscaldamento\nPRESENCE Presenza individuo nella stanza\nTEMP Temperatura rilevata\nNOISE Rumore rilevato\nCHANGE_SETPOINTS Cambiare Set-point\nPRINT_LCD Stampa un messaggio\nQUIT Chiusura\n>")
    if (command == "FAN_ON"): #PUT
      fanOn(address)
    elif (command == "FAN_OFF"): #PUT
      fanOff(address)
    elif (command == "LED_ON"): #PUT
      ledOn(address)
    elif (command == "LED_OFF"): #PUT
      ledOff(address)
    elif (command == "PRESENCE"):#GET
      presence(address)
    elif (command == "NOISE"):#GET
      noise(address)
    elif (command == "TEMP"):#GET
      temp(address)
    elif (command == "PRINT_LCD"):#PUT
      printLCD(address)
    elif (command == "CHANGE_SETPOINTS"):#PUT
      changeSetpoints(address)
    elif (command == "QUIT"):
      print("Terminazione operazioni in corso")
      break
    else:
      print("Errore inserimento comando, riprovare")


if __name__ == "__main__":
  address = "http://localhost:8081/controller"
  implementationMenu(address)
