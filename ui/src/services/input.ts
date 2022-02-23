import _ from 'lodash';

import { robotApi } from './robot';
import { handleUpdateSubscription, RecursivePartial } from './util';


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
    { key: InputSource.MANUAL,     disabled: false,   name: "Server" },
    { key: InputSource.RC,         disabled: false,  name: "Remote controller" },
    { key: InputSource.WEB,        disabled: false,  name: "Browser" },
    { key: InputSource.CONTROLLER, disabled: true,  name: "Game controller" },
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
                await handleUpdateSubscription("/input", "update_steer", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
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
