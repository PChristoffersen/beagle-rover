import logging
import numpy as np
from typing import Any
from aiohttp.web import Request, Response, json_response as _json_response

logger = logging.getLogger(__name__)


try:
    import orjson
    _loads = orjson.loads
    
    def _dumps(*args, **kwargs):
        # Orjson does not support the separators argument
        if "separators" in kwargs:
            kwargs.pop("separators")
        return str(orjson.dumps(*args, **kwargs, option=orjson.OPT_SERIALIZE_NUMPY), "utf-8")

except:
    logger.warn("orjson not found falling back to default json serializer")
    import json
    _dumps = json.dumps
    _loads = json.loads
    


def dumps(*args, **kwargs):
    return _dumps(*args, **kwargs)

def loads(*args, **kwargs):
    return _loads(*args, **kwargs)


async def json_request(request: Request):
    body = await request.read()
    return _loads(body)

def json_response(data: Any, status: int = 200) -> Response:
    return _json_response(body=_dumps(data), status=status)
