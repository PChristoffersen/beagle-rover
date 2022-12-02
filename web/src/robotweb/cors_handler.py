import logging
from aiohttp.web import Request, Response, middleware

logger = logging.getLogger(__name__)

@middleware
async def cors_middleware(request: Request, handler) -> Response:
    response = await handler(request)
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response
