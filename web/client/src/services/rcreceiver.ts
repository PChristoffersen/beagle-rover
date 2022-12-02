import { RCChannels, RCReceiver } from './model';
import { robotApi } from './robot';
import { handleUpdateQuery, handleUpdateSubscription, RecursivePartial } from './util';


const rcreceiverApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getRCReceiver: builder.query<RCReceiver, void>({
            query: () => `rcreceiver`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'RCReceiver' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/rcreceiver", "update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

        setRCReceiver: builder.mutation<RCReceiver, RecursivePartial<RCReceiver>>({
            query(data) {
                return {
                    url: `rcreceiver`,
                    method: 'PUT',
                    body: data,
                }
            },
            async onQueryStarted({ ...patch }, { dispatch, queryFulfilled }) {
                await handleUpdateQuery(undefined, patch, 'getRCReceiver', ['RCReceiver'], dispatch, queryFulfilled, rcreceiverApi.util.updateQueryData, rcreceiverApi.util.invalidateTags);
            },
        }),

        getRCChannels: builder.query<RCChannels, void>({
            query: () => `rcreceiver/channels`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'RCChannels' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/rcreceiver", "update_channels", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetRCReceiverQuery,
    useSetRCReceiverMutation,
    useGetRCChannelsQuery,
} = rcreceiverApi;
