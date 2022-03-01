import { robotApi } from './robot';
import { handleUpdateSubscription } from './util';


export enum PowerSourceType {
    UNKNOWN = "UNKNOWN",
    BATTERY = "BATTERY"
}

export interface PowerSource {
    type: PowerSourceType
}

export interface PowerSourceUnknown extends PowerSource {
    type: PowerSourceType.UNKNOWN
}

export interface PowerSourceBattery extends PowerSource {
    type: PowerSourceType.BATTERY
    charging: boolean,
    on_battery: boolean,
    jack_voltage: number,
    percent: number,
    voltage: number,
}

export interface Power {
    system: PowerSourceUnknown | PowerSourceBattery
}

export interface Network {

}



export interface System {
    network: Network,
    power: Power,
}


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
