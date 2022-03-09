import { robotApi } from './robot';
import { handleUpdateQuery, handleUpdateSubscription, RecursivePartial } from './util';
import { io, Socket } from 'socket.io-client';
import { socketPrefix } from "./robot";
import { InputControl, InputSourceDescriptionList, InputState } from './model';


const inputApi = robotApi.injectEndpoints({
    
    endpoints: (builder) => ({
        getInput: builder.query<InputControl, void>({
            query: () => `input`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'Input' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/input", "update", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),


        setInput: builder.mutation<InputControl, RecursivePartial<InputControl>>({
            query(data) {
                return {
                    url: `input`,
                    method: 'PUT',
                    body: data,
                }
            },
            async onQueryStarted({ ...patch }, { dispatch, queryFulfilled }) {
                await handleUpdateQuery(undefined, patch, 'getInput', ['Input'], dispatch, queryFulfilled, inputApi.util.updateQueryData, inputApi.util.invalidateTags);
            },
        }),


        getInputState: builder.query<InputState, void>({
            query: () => `input/state`,

            // @ts-expect-error
            providesTags: (result, error, id) => [ 'InputState' ],

            async onCacheEntryAdded(arg, { updateCachedData, cacheDataLoaded, cacheEntryRemoved}) {
                await handleUpdateSubscription("/input", "update_state", updateCachedData, cacheDataLoaded, cacheEntryRemoved);
            }
        }),

        setInputState: builder.mutation<InputState, RecursivePartial<InputState>>({
            query(data) {
                return {
                    url: `input/state`,
                    method: 'PUT',
                    body: data,
                }
            },
            async onQueryStarted({ ...patch }, { dispatch, queryFulfilled }) {
                await handleUpdateQuery(undefined, patch, 'getInputState', ['InputState'], dispatch, queryFulfilled, inputApi.util.updateQueryData, inputApi.util.invalidateTags);
            },
        }),

        getInputSources: builder.query<InputSourceDescriptionList, void>({
            query: () => `input/sources`,
            // @ts-expect-error
            providesTags: (result, error, id) => [ 'input/sources' ],
        }),

    }),

    overrideExisting: false,
});


export function createSocket() {
    return io(socketPrefix+"/input");
}

export function steer(sock: Socket, data: RecursivePartial<InputState>) {
    sock.emit("steer", data);
}


// Export hooks for usage in functional components, which are
// auto-generated based on the defined endpoints
export const { 
    useGetInputQuery,
    useSetInputMutation,
    useGetInputStateQuery,
    useSetInputStateMutation,
    useGetInputSourcesQuery,
} = inputApi;
