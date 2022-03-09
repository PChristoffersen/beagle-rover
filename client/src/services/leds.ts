import { AnimationModeDescriptionList, IndicatorModeDescriptionList, LEDControl, LEDOutput } from './model';
import { robotApi } from './robot';
import { handleUpdateQuery, handleUpdateSubscription, RecursivePartial } from './util';


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
                await handleUpdateQuery(undefined, patch, 'getLEDS', ['LEDS'], dispatch, queryFulfilled, ledsApi.util.updateQueryData, ledsApi.util.invalidateTags);
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

        getAnimationModes: builder.query<AnimationModeDescriptionList, void>({
            query: () => `leds/animations`,
            // @ts-expect-error
            providesTags: (result, error, id) => [ 'LEDS/Animations' ],
        }),
        getIndicatorModes: builder.query<IndicatorModeDescriptionList, void>({
            query: () => `leds/indicators`,
            // @ts-expect-error
            providesTags: (result, error, id) => [ 'LEDS/Indicators' ],
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
    useGetAnimationModesQuery,
    useGetIndicatorModesQuery,
} = ledsApi;
