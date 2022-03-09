import { System } from './model';
import { robotApi } from './robot';
import { handleUpdateSubscription } from './util';


const systemApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getSystem: builder.query<System, void>({
            query: () => `system`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'System' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/system", "update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetSystemQuery,
} = systemApi;
