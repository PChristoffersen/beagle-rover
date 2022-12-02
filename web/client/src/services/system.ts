import { Network, Power } from './model';
import { robotApi } from './robot';
import { handleUpdateSubscription } from './util';


const systemApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getPower: builder.query<Power, void>({
            query: () => `system/power`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'System::Power' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/system", "power_update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),
        getNetwork: builder.query<Network, void>({
            query: () => `system/network`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'System::Network' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/system", "network_update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetPowerQuery,
    useGetNetworkQuery,
} = systemApi;
