import _ from 'lodash';
import { io } from 'socket.io-client';

import { robotApi, socketPrefix } from './robot';
import { RecursivePartial } from './util';

export interface MotorServo {
    enabled: boolean,
    angle: number,
    pulse_us: number,
    limit_min: number,
    limit_max: number,
}

export interface Motor {
    id: number,
    enabled: boolean,
    duty: number,
    target_rpm: number,
    rpm: number,
    encoder: number,
    odometer: number,
    servo?: MotorServo,
}




robotApi.enhanceEndpoints({
    addTagTypes: ['Motor', 'MotorServo']
});


const motorApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getMotor: builder.query<Motor, number>({
            query: (id) => `motors/${id}`,

            // @ts-expect-error
            providesTags: (result, error, id) => [{ type: 'Motor', id }],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                const sock = io(socketPrefix+"/motors");
                const watchName = "update_motor_"+arg

                try {
                    sock.on("connect", () => {
                        sock.emit("add_watch", watchName, (answer: RecursivePartial<Motor>) => {
                            updateCachedData((draft) => {
                                _.merge(draft, answer)
                            })
                        })
                    })
                        
                    // wait for the initial query to resolve before proceeding
                    await cacheDataLoaded

                    sock.on(watchName, (data: RecursivePartial<Motor>) => {
                        //console.log("MotorChanged", data)
                        if (data?.id === arg) {
                            updateCachedData((draft) => {
                                _.merge(draft, data)
                            })
                        }
                    })
                    
                } catch {
                    // no-op in case `cacheEntryRemoved` resolves before `cacheDataLoaded`,
                    // in which case `cacheDataLoaded` will throw
                }
   
                await cacheEntryRemoved

                sock.disconnect()
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

            /*
            // @ts-expect-error
            invalidatesTags: (result, error, { id }) => [{ type: 'Motor', id }],
            */

            async onQueryStarted({ id, ...patch }, { dispatch, queryFulfilled }) {
                const patchResult = dispatch(
                    motorApi.util.updateQueryData('getMotor', id, (draft) => {
                        _.merge(draft, patch)
                    })
                )
                try {
                    const { data } = await queryFulfilled
                    if (data?.id === id) {
                        dispatch(motorApi.util.updateQueryData('getMotor', id, (draft) => {
                            _.merge(draft, data)
                        }));
                    }
                } catch {
                    patchResult.undo()
                    // @ts-expect-error
                    dispatch(motorApi.util.invalidateTags([{ type: 'Motor', id }]))
                }
            },

        }),
    }),
    overrideExisting: false,
});

// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetMotorQuery,
    useSetMotorMutation,
} = motorApi;
