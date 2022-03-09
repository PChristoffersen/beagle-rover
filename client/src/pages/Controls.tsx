import { Box, Card, CardContent, CardHeader, Grid, Paper, Stack, Typography, useMediaQuery, useTheme } from '@mui/material';
import RobotFront from '../components/chassis/RobotFront';
import RobotSide from '../components/chassis/RobotSide';
import RobotTop from '../components/chassis/RobotTop';
import InputSourceSelect from '../components/input/InputSourceSelect';
import SteeringCard from '../components/input/SteeringCard';
import DriveModeSelect from '../components/kinematics/DriveModeSelect';
import OrientationSelect from '../components/kinematics/OrientationSelect';

import Page from '../components/Page';
import { useGetInputQuery } from '../services/input';
import { useGetKinematicQuery } from '../services/kinematic';
import { useGetOutputQuery } from '../services/leds';
import { DriveMode, InputSource } from '../services/model';
import { useGetMotorControlQuery } from '../services/motors';

function SettingsCard() {
    return (
        <Card>
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

function ChassisPaper() {
    const { data: leds } = useGetOutputQuery();
    const { data: kinematics } = useGetKinematicQuery();

    const sideView = kinematics?.drive_mode===DriveMode.BALANCING;

    return (
        <Paper>
            { !sideView && <RobotTop leds={leds}/> }
            {  sideView && <RobotSide /> }
        </Paper>
    )
}

function OdometerPaper() {
    const { data } = useGetMotorControlQuery();

    return (
        <Paper>
            <Typography variant="h5" alignItems="center" textAlign="center">
                {data?.odometer}
            </Typography>
        </Paper>

    )
}

function CompassPaper() {
    return (
        <Paper>
            <Typography variant="h5" alignItems="center" textAlign="center">
                Compass
            </Typography>
        </Paper>
    )
}


export default function Controls() {
    const { data: input } = useGetInputQuery();

    const theme = useTheme();
    const isFull = useMediaQuery(theme.breakpoints.up('md'))

    const inControl = input?.axis_source===InputSource.WEB;

    const gridSpacing = 2;
    const gridPadding = 2;

    return (
        <Page>
            <Grid container spacing={gridSpacing} padding={gridPadding}>
                <Grid item xs={2}>
                    <SettingsCard />
                </Grid>
                <Grid item xs={4}>
                    <SteeringCard />
                </Grid>
                <Grid item xs={1}>
                    <Grid container spacing={gridSpacing}>
                        <Grid item xs={12} >
                            <OdometerPaper />
                        </Grid>
                        <Grid item xs={12}>
                            <Paper>
                                <RobotFront />
                            </Paper>
                        </Grid>
                        <Grid item xs={12}>
                            <Paper>
                                <RobotSide />
                            </Paper>
                        </Grid>
                        <Grid item xs={12}>
                            <CompassPaper />
                        </Grid>
                    </Grid>
                </Grid>
                <Grid item xs={5}>
                    <ChassisPaper />
                </Grid>
            </Grid>

        </Page>
    )
}