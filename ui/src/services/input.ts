import _ from 'lodash';
import { io } from 'socket.io-client';

import { robotApi, socketPrefix } from './robot';
import { RecursivePartial } from './util';


export interface InputState {
    controller: string,
    steering: number,
    throttle: number,
    aux_x?: number,
    aux_y?: number,
}

export enum InputSource {
    MANUAL = "MANUAL",
    RC = "RC",
    WEB = "WEB",
    CONTROLLER = "CONTROLLER",
}

export const inputSources = [
    { key: InputSource.MANUAL,     disabled: true,   name: "Manual" },
    { key: InputSource.RC,         disabled: false,  name: "Remote controller" },
    { key: InputSource.WEB,        disabled: false,  name: "Web page" },
    { key: InputSource.CONTROLLER, disabled: false,  name: "Game controller" },
]

export interface Input {
    source: InputSource,
    kinematric_source: InputSource,
    led_source: InputSource,
}



const inputApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getInput: builder.query<Input, void>({
            query: () => `input`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'Input' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                const sock = io(socketPrefix+"/input")
                const watchName = "update"

                try {
                    sock.on("connect", () => {
                        sock.emit("add_watch", watchName, (answer: RecursivePartial<Input>) => {
                            updateCachedData((draft) => {
                                _.merge(draft, answer)
                            })
                        })
                    })

                    // wait for the initial query to resolve before proceeding
                    await cacheDataLoaded

                    sock.on(watchName, (data) => {
                        console.log("Input", data)
                        updateCachedData((draft) => {
                            _.merge(draft, data)
                        })
                    })
                } catch {
                    // no-op in case `cacheEntryRemoved` resolves before `cacheDataLoaded`,
                    // in which case `cacheDataLoaded` will throw
                }
   
                await cacheEntryRemoved

                sock.disconnect()
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
                const patchResult = dispatch(
                    inputApi.util.updateQueryData('getInput', undefined, (draft) => {
                        _.merge(draft, patch)
                    })
                )
                try {
                    const { data } = await queryFulfilled
                    dispatch(inputApi.util.updateQueryData('getInput', undefined, (draft) => {
                        _.merge(draft, data)
                    }));
                } catch {
                    patchResult.undo()
                    // @ts-expect-error
                    dispatch(kinematicApi.util.invalidateTags(['Input']))
                }
            },
        }),


        getInputSteering: builder.query<InputState, void>({
            query: () => `input/steer`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'InputSteer' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                const sock = io("/input")
                const watchName = "update_steer"

                try {
                    sock.on("connect", () => {
                        sock.emit("add_watch", watchName, (answer: RecursivePartial<InputState>) => {
                            updateCachedData((draft) => {
                                _.merge(draft, answer)
                            })
                        })
                    })

                    // wait for the initial query to resolve before proceeding
                    await cacheDataLoaded

                    sock.on(watchName, (data) => {
                        //console.log("Input", data)
                        updateCachedData((draft) => {
                            _.merge(draft, data)
                        })
                    })
                } catch {
                    // no-op in case `cacheEntryRemoved` resolves before `cacheDataLoaded`,
                    // in which case `cacheDataLoaded` will throw
                }
   
                await cacheEntryRemoved
                sock.disconnect()
            }
        }),

    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetInputQuery,
    useSetInputMutation,
    useGetInputSteeringQuery,
} = inputApi;
