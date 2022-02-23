from json import JSONDecodeError
import logging
from aiohttp.web import Request, Response, middleware
from aiohttp.web import HTTPException, HTTPRedirection, HTTPServerError
from .serializer import json_response

logger = logging.getLogger(__name__)

def create_problem_details(status: int, title: str, detail: str = None, type: str = None, instance: str = None) -> dict:
    res = {
        "status": status,
        "title": title,
    }
    if detail:
        res["detail"] = detail
    if type:
        res["type"] = type
    if instance:
        res["instance"] = instance
    return res


@middleware
async def error_middleware(request: Request, handler) -> Response:
    try:
        return await handler(request)

    except HTTPException as ex:
        logger.warn(f"Error {request.method} {request.path}: {ex}")
        return json_response(status=ex.status, data=create_problem_details(ex.status, ex.reason))

    except (JSONDecodeError,ValueError) as ex:
        logger.warn(f"Error {request.method} {request.path}: {ex}")
        return json_response(status=400, data=create_problem_details(400, "Bad request", str(ex)))

    except Exception as ex:
        logger.warn(f"Error {request.method} {request.path}: {ex}", exc_info=1)
        return json_response(status=500, data=create_problem_details(500, "Internal Server Error", str(ex)))
