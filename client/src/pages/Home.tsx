import Typography from '@mui/material/Typography';

import Page from '../components/Page';
import { Grid } from '@mui/material';



export default function Home() {
    return (
        <Page>
            <Grid container spacing={{ xs: 1, sm: 2 }} sx={{ padding: { xs: 1, sm: 2 } }}>
                <Grid item>
                    <Typography variant="h4" component="h4" gutterBottom>
                        Home
                    </Typography>
                </Grid>
            </Grid>
        </Page>
    )
}