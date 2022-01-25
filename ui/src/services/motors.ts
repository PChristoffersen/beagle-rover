import { io } from 'socket.io-client';

import { robotApi } from './robot';

export interface MotorServo {
    id: number,
    enabled: boolean,
    angle: number,
}

export interface Motor {
    id: number,
    enabled: boolean,
    duty: number,
    target_rpm: number,
    rpm: number,
    encoder: number,
    odometer: number,
    servo: MotorServo,
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
                const sock = io("/motors");

                sock.on("connect", () => {
                    sock.emit("watch", { id: arg })
                });


                try {
                    // wait for the initial query to resolve before proceeding
                    await cacheDataLoaded

                    sock.on("updated", (data) => {
                        console.log("MotorChanged", data)
                        updateCachedData((draft) => {
                            Object.assign(draft, data)
                        })
                    })
                } catch {
                    // no-op in case `cacheEntryRemoved` resolves before `cacheDataLoaded`,
                    // in which case `cacheDataLoaded` will throw
                }
   
                await cacheEntryRemoved
                console.log("Socket closed")
                sock.close()
            }
        }),
        setMotor: builder.mutation<Motor, Partial<Motor> & Pick<Motor, 'id'>>({
            query(data) {
                const { id, ...body } = data
                return {
                    url: `motors/${id}`,
                    method: 'PUT',
                    body,
                }
            },
            async onQueryStarted({ id, ...patch }, { dispatch, queryFulfilled }) {
                const patchResult = dispatch(
                    motorApi.util.updateQueryData('getMotor', id, (draft) => {
                        Object.assign(draft, patch)
                    })
                )
                try {
                    await queryFulfilled
                } catch {
                    patchResult.undo()
                    // @ts-expect-error
                    dispatch(extendedApi.util.invalidateTags([{ type: 'Motor', id }]))
                }
            },

            // @ts-expect-error
            invalidatesTags: (result, error, { id }) => [{ type: 'Motor', id }],
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
