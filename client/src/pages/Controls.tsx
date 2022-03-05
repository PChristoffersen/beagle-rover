import { Card, CardContent, CardHeader, Grid, Stack } from '@mui/material';
import TopView from '../components/chassis/beaglerover/TopView';
import InputSourceSelect from '../components/input/InputSourceSelect';
import SteeringCard from '../components/input/SteeringCard';
import DriveModeSelect from '../components/kinematics/DriveModeSelect';
import OrientationSelect from '../components/kinematics/OrientationSelect';

import Page from '../components/Page';
import { InputSource, useGetInputQuery } from '../services/input';


function SettingsCard() {
    return (
        <Card sx={{ minWidth: 200 }}>
            <CardHeader title="Settings" />
            <CardContent>
                <Stack spacing={3}>
                    <InputSourceSelect source="axis_source" />
                    <InputSourceSelect source="kinematic_source" />
                    <InputSourceSelect source="led_source" />
                    <DriveModeSelect />
                    <OrientationSelect />
                </Stack>
            </CardContent>
        </Card>
    )
}

function ChassisCard() {
    return (
        <Card>
            <CardContent>
                <TopView />
            </CardContent>
        </Card>
    )
}


export default function Controls() {
    const { data: input } = useGetInputQuery();

    const inControl = input?.axis_source===InputSource.WEB;

    return (
        <Page>
            <Grid container spacing={{ xs: 1, sm: 2 }} sx={{ padding: { xs: 1, sm: 2 } }}>
                <Grid item xs={12} sm="auto" order={{ xs: 3, sm: 1 }} >
                    <SettingsCard />
                </Grid>
                <Grid item xs={12} sm="auto" order={{ xs: 1, sm: 2 }} visibility={!inControl?"hidden":undefined}>
                    <SteeringCard />
                </Grid>
                <Grid item xs={12} sm={6} order={{ xs: 2, sm: 3 }}>
                    <ChassisCard />
                </Grid>
            </Grid>
        </Page>
    )
}