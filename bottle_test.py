from bottle import route, run, template
import bottle
from datetime import datetime

@route('/time')
def index(name='time'):
    dt = datetime.now()
    time = "{:%Y-%m-%d %H:%M:%S}".format(dt)
    return template('<b>Pi thinks the date/time is: {{t}}</b>', t=time)

@route('/data/<text>')
def index(text):
    return template('<b>{{text}}</b>',text=text)

@bottle.get('/gettest')
def gettest():
    name = bottle.request.query.get('name')
    age = bottle.request.query.get('age')
    return template('<b>name={{name}}, age={{age}}</b>',name=name,age=age)

@bottle.get('/sensor')
def sensor():
    user_key = bottle.request.query.get('ukey')
    sensor_id = bottle.request.query.get('sid')
    sensor_data = bottle.request.query.get('sdata')
    dt = datetime.now()
    time = "{:%Y%m%d%H%M%S}".format(dt)
    return template('<b>{{ukey}}|{{time}}|{{sid}}|{{sdata}}</b>',ukey=user_key,sid=sensor_id,sdata=sensor_data,time=time)

run(host='192.168.10.106', port=80)
