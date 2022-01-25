import logging
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response

from robotsystem import InputSource

from .util import to_enum

logger = logging.getLogger(__name__)

route = RouteTableDef()

INPUT_PROPERTIES = frozenset([
    "source",
])


def input2dict(input) -> dict:
    return {
        "source": str(input.source)
    }

def set_input_from_dict(input, json: dict):
    for key, value in json.items():
        if key in INPUT_PROPERTIES:
            if key == "source":
                value = to_enum(InputSource, value)
            setattr(input, key, value)



@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    input = robot.input
    return json_response(input2dict(input))


@route.put("")
async def put(request: Request) -> Response:
    robot = request.config_dict["robot"]
    input = robot.input
    json = await request.json()
    set_input_from_dict(input, json)
    return json_response(input2dict(input))


@route.get("/sources")
async def animations(request: Request) -> Response:
    return json_response([ str(v) for k,v in InputSource.values.items()])



app = Application()
app.add_routes(route)
