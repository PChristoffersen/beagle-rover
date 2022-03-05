import { Grid } from '@mui/material';
import Typography from '@mui/material/Typography';
import FrontView from '../components/chassis/beaglerover/FrontView';
import SideView from '../components/chassis/beaglerover/SideView';
import TopView from '../components/chassis/beaglerover/TopView';

import Page from '../components/Page';

export default function Telemetry() {
    return (
        <Page>
            <Typography variant="h4" component="h4" gutterBottom>
                Telemetry
            </Typography>
            <Grid container spacing={{ xs: 1, sm: 2 }} sx={{ padding: { xs: 1, sm: 2 } }}>
                <Grid item xs={4} >
                    <SideView />
                </Grid>
                <Grid item xs={4}>
                    <FrontView/>
                </Grid>
                <Grid item xs={4}>
                    <TopView />
                </Grid>
            </Grid>
        </Page>
    )
}