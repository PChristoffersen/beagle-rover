import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import Disconnected from '../pages/Disconnected';


export enum ConnectionState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
}

export interface BackendState {
    connection: ConnectionState,
}

const initialState: BackendState = {
    connection: ConnectionState.DISCONNECTED,
}

export const backendSlice = createSlice({
    name: 'backend',
    initialState,
    reducers: {
        startConnecting: (state => {
            state.connection = ConnectionState.CONNECTING;
        }),
        connectionEstablished: (state => {
            state.connection = ConnectionState.CONNECTED;
        }),
        connectionLost: (state => {
            state.connection = ConnectionState.CONNECTING;
        }),
        disconnect: (state => {
            state.connection = ConnectionState.DISCONNECTED;
        }),
    },
})


// Action creators are generated for each case reducer function
export const backendActions = backendSlice.actions

export default backendSlice.reducer
