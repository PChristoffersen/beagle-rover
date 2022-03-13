import { io } from 'socket.io-client';
import { createApi, fetchBaseQuery } from '@reduxjs/toolkit/query/react'

//export const socketPrefix = "";
//export const socketPrefix = "http://localhost:5000";
export const socketPrefix = "http://beaglebone-blue:5000";



interface Versions {
    version: string;
    robotsystem: string;
}



export const robotSocket = io(socketPrefix);

robotSocket.on("connect", () => {
    console.log("RobotConnect", robotSocket.id);
});
  
robotSocket.on("disconnect", () => {
    console.log("RobotDisconnect", robotSocket.id);
});
  

export const robotApi = createApi({
    reducerPath: 'robotApi',
    baseQuery: fetchBaseQuery({ baseUrl: '/api' }),
    endpoints: (builder) => ({

        getVersions: builder.query<Versions, void>({
            query: () => `versions`,
            // @ts-expect-error
            providesTags: (result, error, id) => [ 'versions' ],
        }),

    }),
})

export const {
    useGetVersionsQuery,
} = robotApi;