import { robotApi } from './robot';
import { handleUpdateQuery, handleUpdateSubscription, RecursivePartial } from './util';
import { io, Socket } from 'socket.io-client';
import { socketPrefix } from "./robot";


export interface InputState {
    controller: string,
    steering: number,
    throttle: number,
    aux_x: number,
    aux_y: number,
}

export enum InputSource {
    MANUAL = "MANUAL",
    RC = "RC",
    WEB = "WEB",
    CONTROLLER = "CONTROLLER",
}

export const inputSources = [
    { key: InputSource.MANUAL,     disabled: false,   name: "Server" },
    { key: InputSource.RC,         disabled: false,  name: "Remote controller" },
    { key: InputSource.WEB,        disabled: false,  name: "Browser" },
    { key: InputSource.CONTROLLER, disabled: true,  name: "Game controller" },
]

export interface Input {
    axis_source: InputSource,
    kinematic_source: InputSource,
    led_source: InputSource,
}



const inputApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getInput: builder.query<Input, void>({
            query: () => `input`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'Input' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/input", "update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),


        setInput: builder.mutation<Input, RecursivePartial<Input>>({
            query(data) {
                return {
                    url: `input`,
                    method: 'PUT',
                    body: data,
                }
            },
            async onQueryStarted({ ...patch }, { dispatch, queryFulfilled }) {
                await handleUpdateQuery(undefined, patch, 'getInput', ['Input'], dispatch, queryFulfilled, inputApi.util.updateQueryData, inputApi.util.invalidateTags);
            },
        }),


        getInputState: builder.query<InputState, void>({
            query: () => `input/state`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'InputState' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/input", "update_state", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

        setInputState: builder.mutation<InputState, RecursivePartial<InputState>>({
            query(data) {
                return {
                    url: `input/state`,
                    method: 'PUT',
                    body: data,
                }
            },
            async onQueryStarted({ ...patch }, { dispatch, queryFulfilled }) {
                await handleUpdateQuery(undefined, patch, 'getInputState', ['InputState'], dispatch, queryFulfilled, inputApi.util.updateQueryData, inputApi.util.invalidateTags);
            },
        }),

    }),

    overrideExisting: false,
});


export function createSocket() {
    return io(socketPrefix+"/input");
}

export function steer(sock: Socket, data: RecursivePartial<InputState>) {
    sock.emit("steer", data);
}


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetInputQuery,
    useSetInputMutation,
    useGetInputStateQuery,
    useSetInputStateMutation,
} = inputApi;
