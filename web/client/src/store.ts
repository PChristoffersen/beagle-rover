import { combineReducers, configureStore } from '@reduxjs/toolkit'
import counterReducer from './features/counterSlice'
import backendReducer from './features/backendSlice'
import { robotApi } from './services/robot'
import socketMiddleware from './services/websocket'


const rootReducer = combineReducers({
    counter: counterReducer,
    backend: backendReducer,
    [robotApi.reducerPath]: robotApi.reducer
})


export const store = configureStore({
        reducer: rootReducer,
        middleware: (getDefaultMiddleware) => getDefaultMiddleware()
            .concat(robotApi.middleware)
            .concat(socketMiddleware),
    })

export type RootState = ReturnType<typeof rootReducer>
export type AppDispatch = typeof store.dispatch


