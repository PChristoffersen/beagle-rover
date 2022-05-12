import { IMUData, OdometerData, TelemetryHistory, TelemetryHistorySet } from './model';
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

        getOdometer: builder.query<OdometerData, void>({
            query: () => `telemetry/odometer`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'TelemetryOdometer' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/telemetry", "update_odometer", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

        getHistory: builder.query<TelemetryHistorySet, Array<string>>({
            query: (names) => {
                return {
                    url: 'telemetry/history',
                    params: { names },
                }
            },

            // @ts-expect-error
            providesTags: (result, error, names) => [{ type: 'TelemetryHistory', names }],
        }),

        getHistorySingle: builder.query<TelemetryHistory, string>({
            query: (name) => `telemetry/history/${name}`,

            // @ts-expect-error
            providesTags: (result, error, name) => [{ type: 'TelemetryHistorySingle', name }],
        }),

    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetIMUQuery,
    useGetOdometerQuery,
    useGetHistoryQuery,
    useGetHistorySingleQuery,
} = telemetryApi;
