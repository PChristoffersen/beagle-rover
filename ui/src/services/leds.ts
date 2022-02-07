import _ from 'lodash';
import { io } from 'socket.io-client';

import { robotApi, socketPrefix } from './robot';
import { RecursivePartial } from './util';

export enum AnimationMode {
    NONE = "NONE",
    HEADLIGHTS = "HEADLIGHTS",
    CONSTRUCTION = "CONSTRUCTION",
    POLICE = "POLICE",
    AMBULANCE = "AMBULANCE",
    KNIGHT_RIDER = "KNIGHT_RIDER",
    RAINBOW = "RAINBOW",
}

export const animationModes = [
    { key: AnimationMode.NONE,         disabled: false, name: "None" },
    { key: AnimationMode.HEADLIGHTS,   disabled: false, name: "Headlights" },
    { key: AnimationMode.CONSTRUCTION, disabled: false, name: "Construction" },
    { key: AnimationMode.POLICE,       disabled: false, name: "Police" },
    { key: AnimationMode.AMBULANCE,    disabled: false, name: "Ambulance" },
    { key: AnimationMode.KNIGHT_RIDER, disabled: false, name: "Knight Rider" },
    { key: AnimationMode.RAINBOW,      disabled: false, name: "Rainbow" },
]


export enum IndicatorMode {
    NONE = "NONE",
    LEFT = "LEFT",
    RIGHT = "RIGHT",
    HAZARD = "HAZARD",
}

export const indicatorModes = [
    { key: IndicatorMode.NONE,   disabled: false, name: "None" },
    { key: IndicatorMode.LEFT,   disabled: false, name: "Turn Left" },
    { key: IndicatorMode.RIGHT,  disabled: false, name: "Turn Right" },
    { key: IndicatorMode.HAZARD, disabled: false, name: "Hazard" },
]

export interface LEDControl {
    background: string,
    animation: AnimationMode,
    indicators: IndicatorMode,
}

export interface ColorSegments {
    front: Array<string>,
    back: Array<string>
}

export interface ColorLayer {
    id: number,
    visible: boolean,
    depth: number,
    segments: ColorSegments,
}



const ledsApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getLEDS: builder.query<LEDControl, void>({
            query: () => `leds`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'LEDS' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                const sock = io(socketPrefix+"/leds")
                const watchName = "update"

                try {
                    sock.on("connect", () => {
                        sock.emit("add_watch", watchName, (answer: RecursivePartial<LEDControl>) => {
                            updateCachedData((draft) => {
                                _.merge(draft, answer)
                            })
                        })
                    })

                    // wait for the initial query to resolve before proceeding
                    await cacheDataLoaded

                    sock.on(watchName, (data) => {
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

        setLEDS: builder.mutation<LEDControl, RecursivePartial<LEDControl>>({
            query(data) {
                return {
                    url: `leds`,
                    method: 'PUT',
                    body: data,
                }
            },

            async onQueryStarted({ ...patch }, { dispatch, queryFulfilled }) {
                const patchResult = dispatch(
                    ledsApi.util.updateQueryData('getLEDS', undefined, (draft) => {
                        _.merge(draft, patch)
                    })
                )
                try {
                    const { data } = await queryFulfilled
                    dispatch(ledsApi.util.updateQueryData('getLEDS', undefined, (draft) => {
                        _.merge(draft, data)
                    }));
                } catch {
                    patchResult.undo()
                    // @ts-expect-error
                    dispatch(ledsApi.util.invalidateTags(['LEDS']))
                }
            },
        })
    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetLEDSQuery,
    useSetLEDSMutation,
} = ledsApi;
