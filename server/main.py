#!/usr/bin/env python3

import sys
sys.path.append('../../robot-control/build')

import logging
from aiohttp import web

from robot import create_application


#logging.basicConfig(level=logging.DEBUG, format='[%(asctime)s] [%(threadName)s] [%(name)-15s] [%(levelname)s] %(message)s', datefmt='%Y-%m-%d %H:%M:%S %z')


FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(name)-15s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%H:%M:%S', level=logging.INFO)
logging.getLogger('aiohttp').setLevel(logging.WARNING)
#logging.getLogger('socketio').setLevel(logging.WARNING)
#logging.getLogger('engineio').setLevel(logging.WARNING)
logger = logging.getLogger('Main')


if __name__ == '__main__':
    web.run_app(create_application(), port=5000)
#    socketio.run(app, host='0.0.0.0')
    #app.run(host = '0.0.0.0',port=5000, debug=False, threaded=True)

