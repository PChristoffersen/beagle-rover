import { Box, Card, CardContent, CardHeader, Typography } from "@mui/material";
import Chassis from "../components/chassis/beaglerover/Chassis";
import Page from "../components/Page";


export default function Test() {
    return (
        <Page>
            <Box
                sx={{
                    backgroundColor: "red",
                    height: "100%",
                }}
            >
                <Typography variant="h1" >
                    Hello world
                </Typography>
            </Box>
        </Page>
    );
}