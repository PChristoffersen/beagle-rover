import _ from 'lodash';

import { robotApi } from './robot';
import { handleUpdateQuery, handleUpdateSubscription, RecursivePartial } from './util';

export enum AnimationMode {
    NONE = "NONE",
    HEADLIGHTS = "HEADLIGHTS",
    CONSTRUCTION = "CONSTRUCTION",
    POLICE = "POLICE",
    AMBULANCE = "AMBULANCE",
    RUNNING_LIGHT = "RUNNING_LIGHT",
    KNIGHT_RIDER = "KNIGHT_RIDER",
    RAINBOW = "RAINBOW",
    RAINBOW_WAVE = "RAINBOW_WAVE",
}

export const animationModes = [
    { key: AnimationMode.NONE,         disabled: false, name: "None" },
    { key: AnimationMode.HEADLIGHTS,   disabled: false, name: "Headlights" },
    { key: AnimationMode.CONSTRUCTION, disabled: false, name: "Construction" },
    { key: AnimationMode.POLICE,       disabled: false, name: "Police" },
    { key: AnimationMode.AMBULANCE,    disabled: false, name: "Ambulance" },
    { key: AnimationMode.RUNNING_LIGHT, disabled: false, name: "Running Light" },
    { key: AnimationMode.KNIGHT_RIDER, disabled: false, name: "Knight Rider" },
    { key: AnimationMode.RAINBOW,      disabled: false, name: "Rainbow" },
    { key: AnimationMode.RAINBOW_WAVE, disabled: false, name: "Rainbow Wave" },
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
    brightness: number,
    animation: AnimationMode,
    indicators: IndicatorMode,
}

export type ColorSegment = Array<String>;

export type ColorSegments = Map<String, ColorSegment>;

export type LEDOutput = ColorSegments;

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
                await handleUpdateSubscription("/leds", "update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
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
        }),

        getOutput: builder.query<LEDOutput, void>({
            query: () => `leds/output`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'LEDS/Output' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/leds", "update_output", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetLEDSQuery,
    useSetLEDSMutation,
    useGetOutputQuery,
} = ledsApi;
