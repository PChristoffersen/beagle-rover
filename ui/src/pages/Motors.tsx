import * as React from 'react';
import Box from '@mui/material/Box';
import { range } from 'lodash';

import Page from '../components/Page';

import { DriveMode, useGetKinematicQuery } from '../services/kinematic';
import { Grid, Stack } from '@mui/material';
import { InputSource, useGetInputQuery } from '../services/input';
import FullMotorCard from '../components/motor/FullMotorCard';



export default function Motors() {
    const { data: kinematic } = useGetKinematicQuery();
    const { data: input } = useGetInputQuery();
    const enabled = kinematic?.drive_mode === DriveMode.NONE && input?.source === InputSource.WEB;

    const numMotors = 4;

    return (
        <Page>
            <Grid container spacing={{ xs: 1, sm: 2 }} sx={{ padding: { xs: 1, sm: 2 } }}>
                {range(numMotors).map((id) => (
                    <Grid key={id} item xs={12} sm="auto">
                        <FullMotorCard key={"motor-"+id} id={id} disabled={!enabled} />
                    </Grid>
                ))}
            </Grid>
        </Page>
    )
}