import { Box, Card, CardContent, CardHeader, CircularProgress, Grid, Stack, Typography } from "@mui/material";
import CircleIcon from '@mui/icons-material/Circle';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';
import { ColorSegment, useGetOutputQuery } from "../../services/leds";


interface SegmentProps {
    name: string,
    colors: ColorSegment,
};

function Segment({name, colors}: SegmentProps) {
    return (
        <Box sx={{ minWidth: 200 }}>
            <Typography variant="overline">
                {name}
            </Typography>
            <Stack direction="row" >
                {colors.map((col, index) => (
                    <CircleIcon key={index} fontSize="large" htmlColor={"#"+col} />
                ))}
            </Stack>
        </Box>
    )
}


export default function OutputCard() {
    const { data: output, isSuccess, isLoading, isError } = useGetOutputQuery();
    
    return (
        <Card sx={{ minWidth: "fit-content" }}>
            <CardHeader title="Output" />
            <CardContent>
                { !isSuccess && 
                    <Box display="flex" justifyContent="center" alignItems="center">
                        { isError && <ErrorOutlineIcon color="error" fontSize="large" /> }
                        { !isError && isLoading && <CircularProgress /> }
                    </Box>
            
                }
                { isSuccess && 
                    <Grid container spacing={3} >
                        { output && Object.entries(output).map(([name, colors]) => (
                            <Grid key={name} item xs="auto">
                                <Segment name={name} colors={colors} />
                            </Grid>
                        ))}
                    </Grid>
                }
            </CardContent>
        </Card>
    )
}