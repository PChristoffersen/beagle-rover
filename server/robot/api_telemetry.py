import logging
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response

route = RouteTableDef()

@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(robot.telemetry.values)

@route.get("/version")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response({ "version": robot.telemetry.values_version })


app = Application()
app.add_routes(route)
