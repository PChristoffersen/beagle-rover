import { Box, Card, CardContent, CardHeader, Grid, Typography, useTheme } from "@mui/material";
import TopViewFull from "../components/chassis/beaglerover/TopViewFull";
import TopView from "../components/chassis/beaglerover/TopView";
import Page from "../components/Page";
import { useGetOutputQuery } from "../services/leds";


export default function Test() {

    return (
        <Page>
            <Box
                sx={{
                    height: "100%",
                    backgroundColor: "blue",
                }}
            >
                <TopView angle={45}/>
            </Box>
        </Page>
    );
}