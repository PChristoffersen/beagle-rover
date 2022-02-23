import logging
from pathlib import Path
from aiohttp.web import Application, RouteTableDef, Request, Response, FileResponse, HTTPNotFound, static
from socketio import AsyncServer

from robotsystem import Robot

from .api import create_app as api_create_app
from .cors_handler import cors_middleware
from . import serializer as _json

logger = logging.getLogger(__name__)


SERVER_ROOT = Path(__file__).parent.resolve().parent
REACT_ROOT = (SERVER_ROOT / Path("../../ui/build")).resolve()


async def app_on_startup(app: Application):
    logger.info("Startup")
    robot = Robot()
    robot.init()
    app['robot'] = robot


async def app_on_cleanup(app: Application):
    logger.info("Cleanup")
    app["robot"] = None


async def app_on_shutdown(app: Application):
    logger.info("Shutdown")
    robot = app["robot"]
    if robot:
        robot.cleanup()


routes = RouteTableDef()

@routes.get('/')
async def index(request: Request) -> Response:
    index = REACT_ROOT / "index.html"
    logger.info("Index")
    if not index.is_file():
        raise HTTPNotFound()
    return FileResponse(REACT_ROOT / "index.html")
    



def create_application() -> Application:
    app = Application(middlewares=[cors_middleware])
    app.on_startup.append(app_on_startup)
    app.on_cleanup.append(app_on_cleanup)
    app.on_shutdown.append(app_on_shutdown)

    sio = AsyncServer(async_handlers = False, cors_allowed_origins='*', json=_json)
    sio.attach(app)
    app["sio"] = sio

    app.add_subapp("/api", api_create_app(app, sio))
    if REACT_ROOT.is_dir():
        app.add_routes([static("/", str(REACT_ROOT), append_version=True)])
    app.add_routes(routes)


    return app
