import { robotApi } from './robot';
import { handleUpdateQuery, handleUpdateSubscription, RecursivePartial } from './util';


export interface RCReceiver {
    enabled: boolean,
    connected: boolean,
    rssi: number,
}


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
        })
    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetRCReceiverQuery,
    useSetRCReceiverMutation,
} = rcreceiverApi;
