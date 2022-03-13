import { Box, Card, CardContent, CardHeader, Grid, Paper, PaperProps, Stack, StackProps, Typography, useMediaQuery, useTheme } from '@mui/material';
import RobotFront from '../components/chassis/RobotFront';
import RobotSide from '../components/chassis/RobotSide';
import RobotTop from '../components/chassis/RobotTop';
import RobotTopFull from '../components/chassis/RobotTopFull';
import InputSourceSelect from '../components/input/InputSourceSelect';
import SteeringCard from '../components/input/SteeringCard';
import DriveModeSelect from '../components/kinematics/DriveModeSelect';
import OrientationSelect from '../components/kinematics/OrientationSelect';

import Page from '../components/Page';
import { useGetKinematicQuery } from '../services/kinematic';
import { useGetOutputQuery } from '../services/leds';
import { DriveMode, InputSource } from '../services/model';
import { useGetMotorControlQuery } from '../services/motors';
import { useGetIMUQuery } from '../services/telemetry';





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
            { !sideView && <RobotTopFull leds={leds}/> }
            {  sideView && <RobotSide /> }
        </Paper>
    )
}



function angleText(value: number|undefined): string {
    if (value === undefined) {
        return ""
    }
    return (value).toLocaleString(undefined, { minimumFractionDigits: 1, maximumFractionDigits: 1 }) + "°"
}

function radiansToDegrees(value: number|undefined): number|undefined {
    if (value === undefined) {
        return undefined;
    }
    return value*180.0/Math.PI;
}

function headingText(value: number|undefined): string|undefined {
    if (value === undefined) {
        return undefined;
    }
    if (value<0.0) {
        value += 360.0;
    }
    return (value).toLocaleString(undefined, { minimumFractionDigits: 0, maximumFractionDigits: 0 }) + "°"
}


function distanceText(value: number|undefined): string {
    if (value === undefined) {
        return ""
    }
    //return (value/1000.0).toLocaleString(undefined, { minimumFractionDigits: 1, maximumFractionDigits: 1 })
    return "2.4"
}


function InfoPaper({ children, ...props }: PaperProps) {
    return (
        <Paper sx={{ flexGrow: "1", height: "100%", position: "relative" }} {...props}>
            {children}
        </Paper> 
    )
}


function OdometerPaper() {
    const { data } = useGetMotorControlQuery();

    return (
        <InfoPaper>
            <Box sx={{ position: "absolute", top: 0, width: "100%", height: "100%" }} display="flex" justifyContent="center" alignItems="center">
                <Typography variant='h4' textAlign="center">
                    {distanceText(data?.odometer)}
                </Typography>
            </Box>
            <Box sx={{ position: "absolute", top: 2, width: "100%" }}>
                <Typography variant="subtitle2" textAlign="center" color="primary">
                    Meters
                </Typography>
            </Box>
        </InfoPaper>
    )
}


function RollPaper() {
    const { data } = useGetIMUQuery();
    const angle = radiansToDegrees(data?.roll);

    return (
        <InfoPaper>
            <Box sx={{ position: "absolute", bottom: 0, width: "100%" }}>
                <RobotFront angle={angle}/>
            </Box>
            <Box sx={{ position: "absolute", top: 2, width: "100%" }}>
                <Typography variant='h6' textAlign="center" >
                    {angleText(angle)}
                </Typography>
            </Box>
        </InfoPaper>
    )    
}


function PitchPaper() {
    const { data } = useGetIMUQuery();
    const angle = radiansToDegrees(data?.pitch);

    return (
        <InfoPaper>
            <Box sx={{ position: "absolute", bottom: 0, width: "100%" }}>
                <RobotSide angle={angle}/>
            </Box>
            
            <Box sx={{ position: "absolute", top: 2, width: "100%" }}>
                <Typography variant='h6' textAlign="center" >
                    {angleText(angle)}
                </Typography>
            </Box>
        </InfoPaper>
    )    
}


function YawPaper() {
    const { data } = useGetIMUQuery();
    const angle = radiansToDegrees(data?.yaw);

    return (
        <InfoPaper>
            <Box sx={{ position: "absolute", bottom: 0, width: "100%" }}>
                <RobotTop angle={angle}/>
            </Box>
            
            <Box sx={{ position: "absolute", top: 2, width: "100%" }}>
                <Typography variant='h6' textAlign="center">
                    {headingText(angle)}
                </Typography>
            </Box>
        </InfoPaper>
    )    
}


function InfoBoxes({ spacing }: StackProps) {
    return (
        <Stack height="100%" direction="column" spacing={spacing}>
            <InfoPaper>
                <OdometerPaper />
            </InfoPaper>
            <YawPaper />
            <PitchPaper />
            <RollPaper />
        </Stack>
    )
}




export default function Controls() {
    const theme = useTheme();
    const isFull = useMediaQuery(theme.breakpoints.up('md'))

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
                <Grid item xs={6}>
                    <Grid container spacing={gridSpacing}>
                        <Grid item xs={2}>
                            <InfoBoxes spacing={gridSpacing} />
                        </Grid>
                        <Grid item xs={10}>
                            <ChassisPaper />
                        </Grid>
                    </Grid>
                </Grid>
            </Grid>

        </Page>
    )
}