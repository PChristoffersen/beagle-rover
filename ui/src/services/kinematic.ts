import _ from 'lodash';
import { io } from 'socket.io-client';

import { robotApi, socketPrefix } from './robot';
import { RecursivePartial } from './util';


export enum DriveMode {
    NONE = "NONE",
    ALL = "ALL",
    FRONT = "FRONT",
    REAR = "REAR",
    SKID = "SKID",
    SPINNING = "SPINNING",
    BALANCING = "BALANCING",
    PASSTHROUGH = "PASSTHROUGH",
}

export const driveModes = [
    { key: DriveMode.NONE,        disabled: false,  name: "None" },
    { key: DriveMode.ALL,         disabled: false,  name: "All wheel steer" },
    { key: DriveMode.FRONT,       disabled: false,  name: "Front wheel steer" },
    { key: DriveMode.REAR,        disabled: false,  name: "Rear wheel steer" },
    { key: DriveMode.SKID,        disabled: false,  name: "Skid stear" },
    { key: DriveMode.SPINNING,    disabled: false,  name: "Spinning" },
    { key: DriveMode.BALANCING,   disabled: false,  name: "Balancing" },
    { key: DriveMode.PASSTHROUGH, disabled: true,   name: "RC Passthrough" },
]

export enum Orientation {
    NORTH = "NORTH",
    SOUTH = "SOUTH",
    EAST = "EAST",
    WEST = "WEST",
}


export const orientations = [
    { key: Orientation.NORTH, name: "North" },
    { key: Orientation.SOUTH, name: "South" },
    { key: Orientation.EAST,  name: "East" },
    { key: Orientation.WEST,  name: "West" },
]

export interface Kinematic {
    drive_mode: DriveMode,
    orientation: Orientation,
}


const kinematicApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getKinematic: builder.query<Kinematic, void>({
            query: () => `kinematic`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'Kinematic' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                const sock = io(socketPrefix+"/kinematic")
                const watchName = "update"

                try {
                    sock.on("connect", () => {
                        sock.emit("add_watch", watchName, (answer: RecursivePartial<Kinematic>) => {
                            updateCachedData((draft) => {
                                _.merge(draft, answer)
                            })
                        })
                    })

                    // wait for the initial query to resolve before proceeding
                    await cacheDataLoaded

                    sock.on(watchName, (data) => {
                        //console.log("Kinematic", data)
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

        setKinematic: builder.mutation<Kinematic, RecursivePartial<Kinematic>>({
            query(data) {
                return {
                    url: `kinematic`,
                    method: 'PUT',
                    body: data,
                }
            },

            async onQueryStarted({ ...patch }, { dispatch, queryFulfilled }) {
                const patchResult = dispatch(
                    kinematicApi.util.updateQueryData('getKinematic', undefined, (draft) => {
                        _.merge(draft, patch)
                    })
                )
                try {
                    const { data } = await queryFulfilled
                    dispatch(kinematicApi.util.updateQueryData('getKinematic', undefined, (draft) => {
                        _.merge(draft, data)
                    }));
                } catch {
                    patchResult.undo()
                    // @ts-expect-error
                    dispatch(kinematicApi.util.invalidateTags(['Kinematic']))
                }
            },
        })
    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetKinematicQuery,
    useSetKinematicMutation,
} = kinematicApi;
