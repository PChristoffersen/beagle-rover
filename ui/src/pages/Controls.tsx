import { Card, CardContent, CardHeader, Grid, Stack } from '@mui/material';
import InputSourceCard from '../components/input/InputSourceCard';
import InputSourceSelect from '../components/input/InputSourceSelect';
import SteeringCard from '../components/input/SteeringCard';
import DriveModeCard from '../components/kinematics/DriveModeCard';
import DriveModeSelect from '../components/kinematics/DriveModeSelect';
import OrientationSelect from '../components/kinematics/OrientationSelect';

import Page from '../components/Page';


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


export default function Controls() {
    return (
        <Page>
            <Grid container spacing={{ xs: 1, sm: 2 }} sx={{ padding: { xs: 1, sm: 2 } }}>
                <Grid item xs={12} sm="auto" >
                    <SettingsCard />
                </Grid>
{/*                 <Grid item xs={12} sm="auto">
                    <InputSourceCard source="axis_source"/>
                </Grid>
                <Grid item xs={12} sm="auto">
                    <DriveModeCard />
                </Grid>
 */}                <Grid item xs={12} sm="auto">
                    <SteeringCard />
                </Grid>
            </Grid>
        </Page>
    )
}