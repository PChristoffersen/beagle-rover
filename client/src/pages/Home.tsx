import Typography from '@mui/material/Typography';

import Page from '../components/Page';
import { Box, Grid, Stack } from '@mui/material';
import RobotRender from '../components/chassis/RobotRender';
import { useGetVersionsQuery } from '../services/robot';



export default function Home() {
    const { data } = useGetVersionsQuery();

    return (
        <Page>
            <Stack
                direction="column"
                alignItems="center"
                spacing={3}
                sx={{ padding: { xs: 1, sm: 2 } }}
            >
                <Typography variant="h2" component="h4" gutterBottom>
                    BeagleRover
                </Typography>
                <RobotRender width={{ xs: "100%", sm: "80%", md: "60%", lg: "1024px" }} />

                {data && 
                    <Typography variant="subtitle1" color="primary">
                        Backend {data.version}, Library {data?.robotsystem}
                    </Typography>
                }
            </Stack>
        </Page>
    )
}