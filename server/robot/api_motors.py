import asyncio
import contextlib
import logging
import threading
import concurrent
from dataclasses import dataclass
from typing import Dict
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncNamespace, AsyncServer
from .util import WatchableNamespace, Watch

from robotsystem import Motor, Subscription


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

    json = motor2dict(motor)
    json["servo"] = servo2dict(motor.servo)
    return json_response(json)



@dataclass
class MotorWatch(Watch):
    motor: Motor
    motor_sub: Subscription
    servo_sub: Subscription
    updated: int
    updated_telemetry: int
    updated_servo: int
    notified: int
    notified_telemetry: int
    notified_servo: int


class MotorNamespace(WatchableNamespace):
    NAME = "/motors"
    WATCH_TYPE = MotorWatch
    UPDATE_GRACE_PERIOD = 0.2

    def __init__(self, app: Application):
        super().__init__()
        self.app = app
        self.n_clients = 0

    def app_started(self):
        robot = self.app["root"]["robot"]
        self._init_watches([(f"update_motor_{m.index}", m, None, None, 0,0,0, 0,0,0) for m in robot.motor_control.motors])


    def app_cleanup(self):
        self._destroy_watches()


    async def on_disconnect(self, sid):
        logger.info(f"Input disconnect  sid={sid}")
        async with self.session(sid) as session:
            self._destroy_session(session)


    async def on_connect(self, sid, environ):
        #logger.info(f"Motor connection  sid={sid} env={environ}")
        logger.info(f"Motor connection  sid={sid}")
        self.n_clients = self.n_clients + 1
        async with self.session(sid) as session:
            self._init_session(session)


    async def on_disconnect(self, sid):
        async with self.session(sid) as session:
            self._destroy_session(session)
        self.n_clients = self.n_clients - 1
        if self.n_clients <= 0:
            logger.info("No more clients")
            self.n_clients = 0


    def _watch_updated(self, watch: MotorWatch):
        logger.info(f"Watch updated {watch.name} {watch.n_watches}")
        if watch.n_watches == 1:
            motor = watch.motor
            logger.info(f"First watch on motor {motor.index}")

            if not watch.motor_sub:
                watch.motor_sub = motor.subscribe(lambda what: self.__on_motor_notify_thread(watch, what))
            if not watch.servo_sub:
                watch.servo_sub = motor.servo.subscribe(lambda what: self.__on_servo_notify_thread(watch, what))
        elif watch.n_watches <= 0:
            logger.info(f"No more watches on motor {watch.motor.index}")
            self.__stop_watch(watch)


    def _watch_destroyed(self, watch: MotorWatch):
        self.__stop_watch(watch)
        watch.motor = None

    def _watch_data(self, watch: MotorWatch):
        data = motor2dict(watch.motor)
        data["servo"] = servo2dict(watch.motor.servo)
        return data


    def __stop_watch(self, watch: MotorWatch):
        if watch.motor_sub:
            watch.motor_sub.unsubscribe()
            watch.motor_sub = None
        if watch.servo_sub:
            watch.servo_sub.unsubscribe()
            watch.servo_sub = None
        if watch.future:
            watch.future.cancel()
            watch.future = None


    def __on_motor_notify_thread(self, watch: MotorWatch, what: str):
        #logger.info(f"notify motor {what} {watch.motor.index}")
        with watch.lock:
            if what=="telemetry": 
                # Only telemetry updated
                watch.updated_telemetry += 1
            else: 
                # Full update
                watch.updated += 1
            if not watch.future:
                watch.future = asyncio.run_coroutine_threadsafe(self.__on_motor_notify(watch), self.app.loop)


    def __on_servo_notify_thread(self, watch: MotorWatch, what: str):
        #logger.info(f"notify servo {watch.motor.index}")
        with watch.lock:
            watch.updated_servo += 1
            if not watch.future:
                watch.future = asyncio.run_coroutine_threadsafe(self.__on_motor_notify(watch), self.app.loop)

    
    async def __on_motor_notify(self, watch: MotorWatch):
        try:
            motor = watch.motor

            while True:
                with watch.lock:
                    update = ( watch.updated, watch.updated_telemetry, watch.updated_servo )
                    to_update = ( watch.updated!=watch.notified, watch.updated_telemetry!=watch.notified_telemetry, watch.updated_servo!=watch.notified_servo )
                    if not any(to_update):
                        watch.future = None
                        break
                    watch.notified, watch.notified_telemetry, watch.notified_servo = update

                data = dict()
                if to_update[0]: # Update motor
                    data = motor2dict(motor)
                elif to_update[1]: # Update motor telemetry
                    data = motor2dict_telemetry(motor)
                if to_update[2]: # Update servo
                    data["servo"] = servo2dict_update(motor.servo)

                if data:
                    data["id"] = motor.index
                    #logger.info(f"Notify: {data}")
                    await self.emit(watch.name, data=data, room=watch.name)

                await asyncio.sleep(self.UPDATE_GRACE_PERIOD)

        except Exception as e:
            logger.error(f"Task {watch.name} failed", exc_info=e)






async def app_on_startup(app: Application):
    logger.info("Startup")
    ns = app["ns"]
    ns.app_started()




async def app_on_cleanup(app: Application):
    logger.info("Cleanup")
    ns = app["ns"]
    ns.app_cleanup()


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
