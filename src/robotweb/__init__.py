import logging
from pathlib import Path
from xmlrpc.client import boolean
from aiohttp.web import Application, RouteTableDef, Request, Response, FileResponse, HTTPNotFound, HTTPBadRequest, static, get
from socketio import AsyncServer

from .api import create_app as api_create_app
from .cors_handler import cors_middleware
from . import serializer as _json

__version__ = "1.0"

logger = logging.getLogger(__name__)


routes = RouteTableDef()

def index_response(request: Request) -> Response:
    file = Path(__file__).parent / "www" / "index.html"
    return FileResponse(file)


@routes.get("/{path:.*}")
async def default(request: Request) -> Response:
    www_dir = Path(__file__).parent / "www"

    try:
        file = (www_dir / request.match_info['path']).resolve()

        # The resolved path should be relative to the www_dir
        relative = file.relative_to(www_dir)

        if file.is_file():
            return FileResponse(file)

        return FileResponse(www_dir / "index.html")
    except ValueError:
        raise HTTPBadRequest()


async def default_index(request: Request) -> Response:
    file = Path(__file__).parent / "data" / "index.html"
    return FileResponse(file)



def create_application() -> Application:
    app = Application(middlewares=[cors_middleware])
    app["version"] = __version__

    sio = AsyncServer(async_handlers = False, cors_allowed_origins='*', json=_json)
    sio.attach(app)
    app["sio"] = sio

    app.add_subapp("/api", api_create_app(app, sio))
    www_dir = Path(__file__).parent / "www"
    logger.info(f"{www_dir}")
    if www_dir.is_dir():
        logger.info("Web App detected")
        app.add_routes([static("/static", str(www_dir / "static"), append_version=True)])
        app.add_routes(routes)
    else:
        logger.info("No web app detected")
        app.add_routes([get("/", default_index)])

    return app

