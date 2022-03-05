import { Box, Card, CardContent, CardHeader, Grid, Typography } from "@mui/material";
import TopView from "../components/chassis/beaglerover/TopView";
import FrontView from "../components/chassis/beaglerover/FrontView";
import SideView from "../components/chassis/beaglerover/SideView";
import Page from "../components/Page";


export default function Test() {
    return (
        <Page>
            <Box
                sx={{
                    height: "100%",
                }}
            >
                <Typography variant="h1" >
                    Hello world
                </Typography>

                <Grid container>
                    <Grid item >
                        <SideView />
                    </Grid>
                    <Grid item>
                        <FrontView />
                    </Grid>
                </Grid>

            </Box>
        </Page>
    );
}