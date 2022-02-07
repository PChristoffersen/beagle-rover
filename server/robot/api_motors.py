import asyncio
import contextlib
import logging
import threading
import concurrent
from dataclasses import dataclass
from typing import Dict
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncNamespace, AsyncServer
from .watches import WatchableNamespace, SubscriptionWatch

from robotsystem import Motor, Servo


logger = logging.getLogger(__name__)


route = RouteTableDef()

MOTOR_PROPERTIES = frozenset([
    "enabled",
    "duty",
    "target_rpm"
])

SERVO_PROPERTIES = frozenset([
    "enabled",
    "angle",
    "pulse_us"
])



def servo2dict(servo) -> Dict:
    return {
        "enabled": servo.enabled,
        "angle": servo.angle,
        "pulse_us": servo.pulse_us,
        "limit_min": servo.limit_min,
        "limit_max": servo.limit_max,
    }

def servo2dict_update(servo) -> Dict:
    return {
        "enabled": servo.enabled,
        "angle": servo.angle,
        "angle_degrees": servo.angle_degrees,
    }

def motor2dict(motor) -> Dict:
    res = {
        "id": motor.index,
        "enabled": motor.enabled,
        "duty": motor.duty,
        "target_rpm": motor.target_rpm,
        "mode": str(motor.mode),
        "rpm": motor.rpm,
        "encoder": motor.encoder,
        "odometer": motor.odometer,
    }
    return res

def motor2dict_telemetry(motor) -> Dict:
    res = {
        "id": motor.index,
        "rpm": motor.rpm,
        "encoder": motor.encoder,
        "odometer": motor.odometer,
    }
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
            logger.info(f"{motor.index} set  {key}={value}")
            setattr(motor, key, value)


@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    res = []
    for motor in robot.motor_control.motors:
        mot = motor2dict(motor)
        mot["servo"] = servo2dict(motor.servo)
        res.append(mot)
    return json_response(res)


@route.get("/{index:\d+}")
async def get_motor(request: Request) -> Response:
    index = int(request.match_info["index"])
    robot = request.config_dict["robot"]
    motor = robot.motor_control.motors[index]

    logger.info(f"GET Motor {index}")

    mot = motor2dict(motor)
    mot["servo"] = servo2dict(motor.servo)
    return json_response(mot)


@route.put("/{index:\d+}")
async def put_motor(request: Request) -> Response:
    robot = request.config_dict["robot"]
    ns = request.config_dict["ns"]
    index = int(request.match_info["index"])
    motor = robot.motor_control.motors[index]

    logger.info(f"PUT Motor {index}")

    json = await request.json()
    set_motor_from_dict(motor, json)
    
    logger.info(f"PUT Motor {index} <<")

    json = motor2dict(motor)
    json["servo"] = servo2dict(motor.servo)
    return json_response(json)



class MotorWatch(SubscriptionWatch):
    SERVO_OFFSET = 1000
    UPDATE_GRACE_PERIOD = 0.1
    
    def data(self):
        data = motor2dict(self.target)
        data["servo"] = servo2dict(self.target.servo)
        return data

    def _target_subscribe(self):
        if not self.sub:
            super()._target_subscribe()
            self.sub = self.target.servo.subscribe(self.sub, self.SERVO_OFFSET)

    async def emit(self, res: tuple):
        data = {
            "id": self.target.index
        }
        for id in res:
            if id == Motor.NOTIFY_DEFAULT:
                data.update(motor2dict(self.target))
            if id == Motor.NOTIFY_TELEMETRY and not Motor.NOTIFY_DEFAULT in res:
                data.update(motor2dict_telemetry(self.target))
            if id == self.SERVO_OFFSET+Servo.NOTIFY_DEFAULT:
                data["servo"] = servo2dict_update(self.target.servo)
        
        logger.info(f"{data}")
        await self.owner.emit(self.name, data=data, room=self.name)
        await asyncio.sleep(self.UPDATE_GRACE_PERIOD)




class MotorNamespace(WatchableNamespace):
    NAME = "/motors"
    WATCH_TYPE = MotorWatch

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app

    async def app_started(self):
        robot = self.app["root"]["robot"]
        await self._init_watches([MotorWatch(self, motor, f"update_motor_{motor.index}") for motor in robot.motor_control.motors])


    async def app_cleanup(self):
        await self._destroy_watches()






async def app_on_startup(app: Application):
    logger.info("Startup")
    ns = app["ns"]
    await ns.app_started()




async def app_on_cleanup(app: Application):
    logger.info("Cleanup")
    ns = app["ns"]
    await ns.app_cleanup()


def create_app(root: Application, sio: AsyncServer) -> Application:
    app = Application()
    app.add_routes(route)
    app.on_startup.append(app_on_startup)
    app.on_cleanup.append(app_on_cleanup)

    app["root"] = root
    app["sio"] = sio

    ns = MotorNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app
