import { Box, Card, CardContent, CardHeader, Grid, Typography, useTheme } from "@mui/material";
import TopView from "../components/chassis/beaglerover/TopView";
import Page from "../components/Page";
import { useGetOutputQuery } from "../services/leds";


export default function Test() {
    const { data: output, isSuccess } = useGetOutputQuery();


    const wheelAngles = [ 0,0,0,0 ];

    return (
        <Page>
            <Box
                sx={{
                    height: "100%",
                    backgroundColor: "blue",
                }}
            >
                <TopView wheelAngles={wheelAngles} leds={output} />
            </Box>
        </Page>
    );
}