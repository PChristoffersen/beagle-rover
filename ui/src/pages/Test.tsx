import { Box, Card, CardContent, CardHeader } from "@mui/material";
import Chassis from "../components/chassis/beaglerover/Chassis";
import Page from "../components/Page";


export default function Test() {
    return (
        <Page>
            <Box
                sx={{
                    display: 'flex',
                    flexWrap: 'wrap',
                    '& > :not(style)': {
                        m: 1,
                        width: 'fit-content',
                        minWidth: 300,
                    },
                }}
            >
                <Card>
                    <CardHeader title="Chassis" />
                    <CardContent>
                        <Box sx={{ }}>
                            <Chassis />
                        </Box>
                    </CardContent>
                </Card>
            </Box>
        </Page>
    );
}