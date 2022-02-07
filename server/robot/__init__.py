import logging
import socketio
from pathlib import Path
from aiohttp.web import Application, RouteTableDef, Request, Response, FileResponse, static

from robotsystem import Robot

from .api import create_app as api_create_app


logger = logging.getLogger(__name__)


STATIC_ROOT = (Path(__file__).parent / Path("../../ui/build")).resolve()


async def app_on_startup(app: Application):
    logger.info("Startup")
    robot = Robot()
    robot.init()
    app['robot'] = robot


async def app_on_cleanup(app: Application):
    logger.info("Cleanup")
    robot = app["robot"]
    if robot:
        robot.cleanup()

    app["robot"] = None


routes = RouteTableDef()

@routes.get('/')
async def index(request: Request) -> Response:
    logger.info("Index")
    return FileResponse(STATIC_ROOT / "index.html")


def create_application() -> Application:
    print(STATIC_ROOT)
    #exit(1)

    app = Application()
    app.on_startup.append(app_on_startup)
    app.on_cleanup.append(app_on_cleanup)

    sio = socketio.AsyncServer(async_handlers = False, cors_allowed_origins='*')
    sio.attach(app)
    app["sio"] = sio

    app.add_subapp("/api", api_create_app(app, sio))
    if STATIC_ROOT.is_dir():
        app.add_routes(routes)
        app.add_routes([static("/", str(STATIC_ROOT), append_version=True)])


    return app
