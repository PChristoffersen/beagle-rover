import { Box, CircularProgress, Grid, Stack, Theme, Typography } from "@mui/material";
import { SxProps, SystemProps } from '@mui/system';
import CircleIcon from '@mui/icons-material/Circle';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';
import { useGetOutputQuery } from "../../services/leds";
import { Color, ColorSegment } from "../../services/model";


interface SegmentProps {
    name: string,
    colors: ColorSegment,
};

function Segment({ name, colors }: SegmentProps) {
    return (
        <Box sx={{ minWidth: 200 }}>
            <Typography variant="overline">
                {name}
            </Typography>
            <Stack direction="row" >
                {colors.map((col: Color, index: number) => (
                    <CircleIcon key={index} fontSize="large" htmlColor={"#"+col} />
                ))}
            </Stack>
        </Box>
    )
}


export default function Output(props: SystemProps<Theme> & { sx?: SxProps<Theme> }) {
    const { data: output, isSuccess, isLoading, isError } = useGetOutputQuery();

    return (
        <Box width={{ xs: "min-content", sm: "max-content" }}>
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
        </Box>
    )
}