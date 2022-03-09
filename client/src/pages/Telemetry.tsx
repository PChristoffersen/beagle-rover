import { Grid } from '@mui/material';
import Typography from '@mui/material/Typography';
import RobotFront from '../components/chassis/RobotFront';
import RobotSide from '../components/chassis/RobotSide';
import RobotTop from '../components/chassis/RobotTop';

import Page from '../components/Page';
import { useGetIMUQuery } from '../services/telemetry';

export default function Telemetry() {
    const { data: imu } = useGetIMUQuery();

    const pitch = -(imu?.pitch || 0.0)*180.0/Math.PI;
    const roll = (imu?.roll || 0.0)*180.0/Math.PI;
    const yaw = (imu?.yaw || 0.0)*180.0/Math.PI;
    /*
    const pitch = 0;
    const roll = 0;
    const yaw = 0;
    */

    return (
        <Page>
            <Typography variant="h4" component="h4" gutterBottom>
                Telemetry
            </Typography>
            <Grid container spacing={{ xs: 1, sm: 2 }} sx={{ padding: { xs: 1, sm: 2 } }}>
                <Grid item xs={4} >
                    <RobotSide angle={pitch} />
                    {pitch}
                </Grid>
                <Grid item xs={4}>
                    <RobotFront angle={roll}/>
                    {roll}
                </Grid>
                <Grid item xs={4}>
                    <RobotTop/>
                    {yaw}
                </Grid>
            </Grid>
        </Page>
    )
}