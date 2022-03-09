import { DriveModeDescriptionList, KinematicControl, OrientationDescriptionList } from './model';
import { robotApi } from './robot';
import { handleUpdateQuery, handleUpdateSubscription, RecursivePartial } from './util';


const kinematicApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getKinematic: builder.query<KinematicControl, void>({
            query: () => `kinematic`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'Kinematic' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/kinematic", "update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

        setKinematic: builder.mutation<KinematicControl, RecursivePartial<KinematicControl>>({
            query(data) {
                return {
                    url: `kinematic`,
                    method: 'PUT',
                    body: data,
                }
            },
            async onQueryStarted({ ...patch }, { dispatch, queryFulfilled }) {
                await handleUpdateQuery(undefined, patch, 'getKinematic', ['Kinematic'], dispatch, queryFulfilled, kinematicApi.util.updateQueryData, kinematicApi.util.invalidateTags);
            },
        }),

        getDriveModes: builder.query<DriveModeDescriptionList, void>({
            query: () => `kinematic/drive-modes`,
            // @ts-expect-error
            providesTags: (result, error, id) => [ 'Kinematic/DriveModes' ],
        }),
        getOrientations: builder.query<OrientationDescriptionList, void>({
            query: () => `kinematic/orientations`,
            // @ts-expect-error
            providesTags: (result, error, id) => [ 'Kinematic/Orientations' ],
        }),
    }),

    overrideExisting: false,
});


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetKinematicQuery,
    useSetKinematicMutation,
    useGetDriveModesQuery,
    useGetOrientationsQuery,
} = kinematicApi;
