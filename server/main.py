#!/usr/bin/env python3

from ast import Mod
import sys
import logging
from aiohttp import web
from pathlib import Path


#print(sys.path)
#exit(1)

#ROBOTSYSTEM_PATH="../../robotsystem/build"
#ROBOTSYSTEM_FULL_PATH=str((Path(__file__).parent / Path(ROBOTSYSTEM_PATH)).resolve())
#sys.path.append(ROBOTSYSTEM_FULL_PATH)
try:
    import robotsystem
except ModuleNotFoundError:
    print(f"Failed to find 'robotsystem' library in {ROBOTSYSTEM_FULL_PATH}", file=sys.stderr)
    exit(1)


from robot import create_application


FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] %(name)s: %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%H:%M:%S', level=logging.INFO)
logging.getLogger('aiohttp').setLevel(logging.WARNING)
#logging.getLogger('socketio').setLevel(logging.WARNING)
#logging.getLogger('engineio').setLevel(logging.WARNING)
logger = logging.getLogger('Main')



if __name__ == '__main__':
    web.run_app(create_application(), port=5000)
#    socketio.run(app, host='0.0.0.0')
    #app.run(host = '0.0.0.0',port=5000, debug=False, threaded=True)
    pass
