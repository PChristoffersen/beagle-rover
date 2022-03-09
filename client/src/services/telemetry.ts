import { IMUData } from './model';
import { robotApi } from './robot';
import { handleUpdateSubscription } from './util';



const telemetryApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getIMU: builder.query<IMUData, void>({
            query: () => `telemetry/imu`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'TelemetryIMU' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/telemetry", "update_imu", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetIMUQuery,
} = telemetryApi;
