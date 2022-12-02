import logging
import setproctitle
from pathlib import Path
from aiohttp import web
from robotweb import create_application


def main():
    FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-10s] [%(levelname)s] %(name)s: %(message)s'
    logging.basicConfig(format=FORMAT, datefmt='%H:%M:%S', level=logging.INFO)
    logging.getLogger('aiohttp').setLevel(logging.WARNING)
    #logging.getLogger('socketio').setLevel(logging.WARNING)
    #logging.getLogger('engineio').setLevel(logging.WARNING)
    logger = logging.getLogger('Main')

    setproctitle.setproctitle("RobotWeb")
    web.run_app(create_application(), port=5000)


if __name__ == '__main__':
    main()
