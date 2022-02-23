import _ from 'lodash';

import { robotApi } from './robot';
import { handleUpdateSubscription, RecursivePartial } from './util';


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
    { key: Orientation.NORTH, name: "Default" },
    { key: Orientation.SOUTH, name: "Reverse" },
    { key: Orientation.EAST,  name: "Left" },
    { key: Orientation.WEST,  name: "Right" },
]

export function orientationName(orientation: Orientation): string|undefined {
    return orientations.find((value) => value.key===orientation)?.name;
}



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
                await handleUpdateSubscription("/kinematic", "update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
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
