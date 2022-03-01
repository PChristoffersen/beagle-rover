import { Grid } from '@mui/material';
import Typography from '@mui/material/Typography';

import Page from '../components/Page';

export default function Telemetry() {
    return (
        <Page>
            <Grid container spacing={{ xs: 1, sm: 2 }} sx={{ padding: { xs: 1, sm: 2 } }}>
                <Grid item>
                    <Typography variant="h4" component="h4" gutterBottom>
                        Telemetry
                    </Typography>
                </Grid>
            </Grid>
        </Page>
    )
}