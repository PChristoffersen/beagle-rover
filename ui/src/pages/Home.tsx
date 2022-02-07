import * as React from 'react';
import Box from '@mui/material/Box';
import Typography from '@mui/material/Typography';

import Page from '../components/Page';



export default function Home() {
    const index = 0

    return (
        <Page>
            <Typography variant="h4" component="h4" gutterBottom>
                Hello 
            </Typography>
            <Box
                sx={{
                    display: 'flex',
                    flexWrap: 'wrap',
                    '& > :not(style)': {
                    m: 1,
                    width: 128,
                    },
                }}
            >
            </Box>
        </Page>
    )
}