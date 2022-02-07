import * as React from 'react';
import Box from '@mui/material/Box';
import { range } from 'lodash';

import Page from '../components/Page';

import MotorCard from '../components/motor/MotorCard';
import ServoCard from '../components/motor/ServoCard';
import { DriveMode, useGetKinematicQuery } from '../services/kinematic';
import { Stack } from '@mui/material';
import { InputSource, useGetInputQuery } from '../services/input';



export default function Motors() {
    const { data: kinematic } = useGetKinematicQuery();
    const { data: input } = useGetInputQuery();
    const enabled = kinematic?.drive_mode === DriveMode.NONE && input?.source === InputSource.WEB;

    const numMotors = 4;

    return (
        <Page>
            <Box
                sx={{
                    display: 'flex',
                    flexWrap: 'wrap',
                    '& > :not(style)': {
                    m: 1,
                    width: 'fit-content',
                    },
                }}
            >
                {range(numMotors).map((id) => (
                    <Stack 
                        key={id} 
                        spacing={2} 
                    >
                        <MotorCard key={"motor-"+id} id={id} disabled={!enabled} />
                        <ServoCard key={"servo-"+id} id={id} disabled={!enabled} />
                    </Stack>
                ))}
            </Box>
        </Page>
    )
}