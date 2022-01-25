import logging
import socketio
from aiohttp import web

from robotsystem import Robot

from .api import create_app as api_create_app

logger = logging.getLogger(__name__)


async def app_on_startup(app: web.Application):
    logger.info("Startup")
    robot = Robot()
    robot.init()
    app['robot'] = robot


async def app_on_cleanup(app: web.Application):
    logger.info("Cleanup")
    robot = app["robot"]
    if robot:
        robot.cleanup()

    app["robot"] = None


def create_application() -> web.Application:
    app = web.Application()
    app.on_startup.append(app_on_startup)
    app.on_cleanup.append(app_on_cleanup)

    sio = socketio.AsyncServer(async_handlers = False)
    sio.attach(app)
    app["sio"] = sio

    app.add_subapp("/api", api_create_app(app, sio))
    #app.add_routes([web.static("/", "robot-fe/build", append_version=True)])


    return app
