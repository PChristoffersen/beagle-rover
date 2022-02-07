import _ from 'lodash';
import { io } from 'socket.io-client';

import { robotApi, socketPrefix } from './robot';
import { RecursivePartial } from './util';


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
                const sock = io(socketPrefix+"/system")
                const watchName = "update"

                try {
                    sock.on("connect", () => {
                        sock.emit("add_watch", watchName, (answer: RecursivePartial<System>) => {
                            updateCachedData((draft) => {
                                _.merge(draft, answer)
                            })
                        })
                    })

                    // wait for the initial query to resolve before proceeding
                    await cacheDataLoaded

                    sock.on(watchName, (data) => {
                        //console.log("System", data)
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

    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetSystemQuery,
} = systemApi;
