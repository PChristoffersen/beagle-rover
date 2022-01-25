import asyncio
import logging
from typing import Dict
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncNamespace, AsyncServer

logger = logging.getLogger(__name__)

my_rpm = 1


class MotorNamespace(AsyncNamespace):
    NAME = "/motors"

    def __init__(self, app: Application, sio: AsyncServer):
        super().__init__(namespace=MotorNamespace.NAME)
        self.app = app
        self.sio = sio
        self.n_clients = 0
        self.task = None

        @sio.on("*")
        async def on_event(event, sid, data):
            logger.info(f"Event: {event} sid={sid}  data={data}")



    async def telemetry_task(self):
        global my_rpm
        while True:
            #logger.info("Ping")
            await asyncio.sleep(1)
            data = {
                "id": 0,
                "rpm": my_rpm
            }
            await self.sio.emit("motor", data=data, namespace=MotorNamespace.NAME)
            my_rpm = my_rpm+1


    def on_connect(self, sid, environ):
        #logger.info(f"Motor connection  sid={sid} env={environ}")
        logger.info(f"Motor connection  sid={sid}")
        self.n_clients = self.n_clients + 1
        #if self.task == None:
        #    self.task =self.app.loop.create_task(timer(self.asio))
    
    def on_disconnect(self, sid):
        logger.info(f"Motor disconnected  sid={sid}")
        self.n_clients = self.n_clients - 1
        if self.n_clients <= 0:
            logger.info("No more clients")
            self.n_clients = 0
            if self.task:
                self.task.cancel()
                self.task = None

    def on_watch(self, sid, data):
        logger.info(f"Motor watch  sid={sid}  data={data}")
        room = f"motor{data['id']}"
        self.sio.enter_room(sid, room, namespace=self.NAME)




route = RouteTableDef()

MOTOR_PROPERTIES = frozenset([
    "enabled",
    "duty",
    "target_rpm"
])

SERVO_PROPERTIES = frozenset([
    "enabled",
    "angle",
    "angle_degrees",
    "pulse_us"
])



def servo2dict(servo) -> Dict:
    return {
        "enabled": servo.enabled,
        "angle": servo.angle
    }

def motor2dict(motor) -> Dict:
    res = {
        "id": motor.index,
        "enabled": motor.enabled,
        "duty": motor.duty,
        "target_rpm": motor.target_rpm,
        "mode": str(motor.mode),
        "rpm": my_rpm, #motor.rpm,
        "encoder": motor.encoder,
        "odometer": motor.odometer,
    }
    if motor.servo:
        res["servo"] = servo2dict(motor.servo)
    return res


def set_servo_from_dict(servo, json: dict) -> None:
    for key, value in json.items():
        if key in SERVO_PROPERTIES:
            setattr(servo, key, value)


def set_motor_from_dict(motor, json: dict) -> None:
    for key, value in json.items():
        if key == "servo":
            set_servo_from_dict(motor.servo, value)
        elif key in MOTOR_PROPERTIES:
            setattr(motor, key, value)


@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    res = []
    for motor in robot.motor_control.motors:
        res.append(motor2dict(motor))
    return json_response(res)


@route.get("/{index:\d+}")
async def motor(request: Request) -> Response:
    index = int(request.match_info["index"])
    robot = request.config_dict["robot"]
    motor = robot.motor_control.motors[index]

    logger.info(f"GET Motor {index}")

    return json_response(motor2dict(motor))


@route.put("/{index:\d+}")
async def motor(request: Request) -> Response:
    robot = request.config_dict["robot"]
    sio = request.config_dict["sio"]
    index = int(request.match_info["index"])
    motor = robot.motor_control.motors[index]

    logger.info(f"PUT Motor {index}")


    json = await request.json()
    logging.warning(json)
    set_motor_from_dict(motor, json)
    res = motor2dict(motor)
    
    await sio.emit("motor", data=res, room=f"motor{index}", namespace=MotorNamespace.NAME)
    
    return json_response(res)




async def app_on_startup(app: Application):
    robot = app["root"]["robot"]
    logger.info("Startup")
    logger.info(robot)



async def app_on_cleanup(app: Application):
    logger.info("Cleanup")


def create_app(root: Application, sio: AsyncServer) -> Application:
    app = Application()
    app.add_routes(route)
    app.on_startup.append(app_on_startup)
    app.on_cleanup.append(app_on_cleanup)

    app["root"] = root
    app["sio"] = sio

    ns = MotorNamespace(app, sio)
    sio.register_namespace(ns)

    return app
