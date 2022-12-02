import { Box, Card, CardContent, CardHeader } from "@mui/material";
import Output from "./Output";




export default function OutputCard() {
    return (
        <Card sx={{ minWidth: "fit-content" }}>
            <CardHeader title="Output" />
            <CardContent>
                <Box display="flex" justifyContent="center" width="100%">
                    <Output/>
                </Box>
            </CardContent>
        </Card>
    )
}