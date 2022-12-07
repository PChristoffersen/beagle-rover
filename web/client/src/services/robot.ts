import { createApi, fetchBaseQuery } from '@reduxjs/toolkit/query/react'


interface Versions {
    version: string;
    beaglerover: string;
}



export const robotApi = createApi({
    reducerPath: 'robotApi',
    baseQuery: fetchBaseQuery({ baseUrl: '/api' }),
    endpoints: (builder) => ({

        getVersions: builder.query<Versions, void>({
            query: () => `versions`,
            // @ts-expect-error
            providesTags: (result, error, id) => [ 'versions' ],
        }),

    }),
})

export const {
    useGetVersionsQuery,
} = robotApi;