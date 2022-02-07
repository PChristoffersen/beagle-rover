import _ from 'lodash';
import { io } from 'socket.io-client';

import { robotApi, socketPrefix } from './robot';
import { RecursivePartial } from './util';


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
                const sock = io(socketPrefix+"/rcreceiver")
                const watchName = "update"

                try {
                    sock.on("connect", () => {
                        sock.emit("add_watch", watchName, (answer: RecursivePartial<RCReceiver>) => {
                            updateCachedData((draft) => {
                                _.merge(draft, answer)
                            })
                        })
                    })

                    // wait for the initial query to resolve before proceeding
                    await cacheDataLoaded

                    sock.on(watchName, (data) => {
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

        setRCReceiver: builder.mutation<RCReceiver, RecursivePartial<RCReceiver>>({
            query(data) {
                return {
                    url: `rcreceiver`,
                    method: 'PUT',
                    body: data,
                }
            },

            async onQueryStarted({ ...patch }, { dispatch, queryFulfilled }) {
                const patchResult = dispatch(
                    rcreceiverApi.util.updateQueryData('getRCReceiver', undefined, (draft) => {
                        _.merge(draft, patch)
                    })
                )
                try {
                    const { data } = await queryFulfilled
                    dispatch(rcreceiverApi.util.updateQueryData('getRCReceiver', undefined, (draft) => {
                        _.merge(draft, data)
                    }));
                } catch {
                    patchResult.undo()
                    // @ts-expect-error
                    dispatch(rcreceiverApi.util.invalidateTags(['RCReceiver']))
                }
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
