import cherrypy
import json
import time
from classi_web_sw2_1 import*

if __name__=='__main__':
	conf = {
		'/': {
			'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
			'tool.session.on': True
		}
	}
	cherrypy.tree.mount(br(), '/broker', conf)
	cherrypy.tree.mount(us(), '/users', conf)
	cherrypy.tree.mount(ser(), '/services', conf)
	cherrypy.tree.mount(dev(), '/devices', conf)
	cherrypy.engine.start()
	cherrypy.engine.block()
