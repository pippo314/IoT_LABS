import cherrypy
import json
from sw3_4_RemoteController import *

if __name__=='__main__':
	conf = {
		'/': {
			'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
			'tool.session.on': True
		}
	}
	
	cherrypy.config.update({'server.socket_host':'0.0.0.0','server.socket_port': 8081})
#	cherrypy.tree.mount(RemoteController("MySubscriber 1", "test.mosquitto.org", 1883), '/controller', conf)
	cherrypy.tree.mount(RemoteController(), '/controller', conf)
	cherrypy.engine.start()
	cherrypy.engine.block()