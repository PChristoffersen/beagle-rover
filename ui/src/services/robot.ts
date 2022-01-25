import { io } from 'socket.io-client';
import { createApi, fetchBaseQuery } from '@reduxjs/toolkit/query/react'


export const robotSocket = io();

robotSocket.on("connect", () => {
    console.log("Connect", robotSocket.id);
});
  
robotSocket.on("disconnect", () => {
    console.log("Disconnect", robotSocket.id);
});
  

export const robotApi = createApi({
    reducerPath: 'robotApi',
    baseQuery: fetchBaseQuery({ baseUrl: '/api' }),
    endpoints: () => ({}),
})
