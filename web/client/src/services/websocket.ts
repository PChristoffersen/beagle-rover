import { Middleware } from "@reduxjs/toolkit";
import { io, Socket } from "socket.io-client";
import { backendActions } from "../features/backendSlice";


//export const socketPrefix = "";
//export const socketPrefix = "http://localhost:5000";
export const socketPrefix = "http://localhost:5000";
//export const socketPrefix = "http://beaglebone-blue:5000";


const socketMiddleware: Middleware = store => {
    let socket: Socket|null;

    return next => action => {
        if (backendActions.startConnecting.match(action)) {
            if (!socket) {
                socket = io(socketPrefix);

                socket.on("connect", () => {
                    console.log("RobotConnect", socket?.id);
                    store.dispatch(backendActions.connectionEstablished());
                });
                
                socket.on("disconnect", () => {
                    console.log("RobotDisconnect", socket?.id);
                    store.dispatch(backendActions.connectionLost());
                });
            }
        }
        if (backendActions.disconnect.match(action)) {
            if (socket) {
                socket = null;
            }
        }

        next(action);
    }
}

   
export default socketMiddleware;
