import cherrypy
from second_sw import*

if __name__=="__main__":
    conf={
         '/':{
               'request.dispatch':cherrypy.dispatch.MethodDispatcher(),
               'tool.session.on':True
           }
        }
    cherrypy.tree.mount(MyWebServices(), '/', conf)
    cherrypy.config.update({'server.socket_host':'0.0.0.0'})
    cherrypy.config.update({'server.socket_port':8080})

    cherrypy.engine.start()
    cherrypy.engine.block()
