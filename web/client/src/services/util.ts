import _ from 'lodash';
import { ActionCreatorWithPayload, AnyAction, ThunkDispatch } from '@reduxjs/toolkit';
import { RootState } from '@reduxjs/toolkit/dist/query/core/apiState';
import { PromiseWithKnownReason } from '@reduxjs/toolkit/dist/query/core/buildMiddleware/types';
import { PatchCollection, Recipe, UpdateQueryDataThunk } from '@reduxjs/toolkit/dist/query/core/buildThunks';
import { TagDescription } from '@reduxjs/toolkit/dist/query/endpointDefinitions';
import { io } from 'socket.io-client';

import { socketPrefix } from "./websocket";


export type RecursivePartial<T> = {
    [P in keyof T]?: RecursivePartial<T[P]>;
};


export async function handleUpdateSubscription<ResultType>(
    socketUri: string, 
    watchName: string, 
    updateCachedData: (updateRecipe: Recipe<ResultType>) => PatchCollection,
    cacheDataLoaded: PromiseWithKnownReason<any, any>,
    cacheEntryRemoved: Promise<void>
) {
    const sock = io(socketPrefix+socketUri)

    try {
        sock.on("connect", () => {
            sock.emit("add_watch", watchName, (answer: RecursivePartial<ResultType>) => {
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
};



export async function handleUpdateQuery<IdType, ResultType>(
    id: IdType, 
    patch: RecursivePartial<ResultType>, 
    endpointName: string, 
    tags: any,
    dispatch: ThunkDispatch<any, any, AnyAction>,
    queryFulfilled: PromiseWithKnownReason<any, any>,
    updateQueryData: UpdateQueryDataThunk<any, RootState<any, string, any>>,
    invalidateTags: ActionCreatorWithPayload<Array<TagDescription<any>>, string>
    ) 
{
    const patchResult = dispatch(updateQueryData(endpointName, id, (draft) => {
        _.merge(draft, patch);
    }));
    try {
        const { data } = await queryFulfilled;
        dispatch(updateQueryData(endpointName, id, (draft) => {
            _.merge(draft, data);
        }));
    } catch {
        patchResult.undo();
        dispatch(invalidateTags(tags));
    }
}
