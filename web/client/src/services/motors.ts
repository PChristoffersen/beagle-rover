import { Motor, MotorControl, ResetOdometerAction } from './model';
import { robotApi } from './robot';
import { handleUpdateQuery, handleUpdateSubscription, RecursivePartial } from './util';


robotApi.enhanceEndpoints({
    addTagTypes: ['Motor', 'MotorServo']
});


const motorApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getMotorControl: builder.query<MotorControl, void>({
            query: () => `motors`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'Motor/Control' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/motors", "update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

        motorControlAction: builder.mutation<ResetOdometerAction, void>({
            query: (data) => {
                return {
                    url: `motors`,
                    method: 'POST',
                    data,
                }
            },
        }),

        getMotor: builder.query<Motor, number>({
            query: (id) => `motors/${id}`,

            // @ts-expect-error
            providesTags: (result, error, id) => [{ type: 'Motor', id }],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/motors", "update_motor_"+arg, updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),


        setMotor: builder.mutation<Motor, RecursivePartial<Motor> & Pick<Motor, 'id'>>({
            query(data) {
                const { id, ...body } = data
                return {
                    url: `motors/${id}`,
                    method: 'PUT',
                    body,
                }
            },
            async onQueryStarted({ id, ...patch }, { dispatch, queryFulfilled }) {
                await handleUpdateQuery(id, patch, 'getMotor', [{ type: 'Motor', id }], dispatch, queryFulfilled, motorApi.util.updateQueryData, motorApi.util.invalidateTags);
            },
        }),
    }),
    overrideExisting: false,
});

// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetMotorControlQuery,
    useGetMotorQuery,
    useSetMotorMutation,
    useMotorControlActionMutation,
} = motorApi;
