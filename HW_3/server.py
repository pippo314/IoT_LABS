import cherrypy
import json
import os
import requests


class WebServer(object):

    def __init__(self):
        self.r = []

    exposed = True

    def POST(self, *uri, **params):
        body = cherrypy.request.body.read()
        self.r.append(body)


    def GET(self, *uri, **params):
        return self.r


if __name__ == '__main__':
    conf = {
        '/': {
            'tools.sessions.on': True,
            'request.dispatch': cherrypy.dispatch.MethodDispatcher(), 
            }
        }

    cherrypy.tree.mount (WebServer(), '/log', conf)
    cherrypy.config.update({'server.socket_host':'0.0.0.0'})
    cherrypy.config.update({'server.socket_port':8080})

    cherrypy.engine.start()
    cherrypy.engine.block()