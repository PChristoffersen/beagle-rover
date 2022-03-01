import { Grid, IconButton, Stack, Tooltip } from "@mui/material";
import AxisSlider from "./AxisSlider";
import { GiCancel, GiStopSign, GiSteeringWheel, GiJoystick } from "react-icons/gi";
import { useSetInputStateMutation } from "../../services/input";

export default function SliderSteering() {
    const [ update, status ] = useSetInputStateMutation();
    const iconSize = 46;

    return (
        <Grid container rowSpacing={3}>
            <Grid item xs="auto">
                <Tooltip title="Stop all">
                    <IconButton disabled={status.isLoading} onClick={() => update({ steering: 0.0, throttle: 0.0, aux_x: 0.0, aux_y: 0.0 }) }>
                        <GiCancel size={iconSize} />
                    </IconButton>
                </Tooltip>
            </Grid>
            <Grid item xs="auto">
                <Tooltip title="Center steering">
                    <IconButton disabled={status.isLoading} onClick={() => update({ steering: 0.0 }) }>
                        <GiSteeringWheel size={iconSize} />
                    </IconButton>
                </Tooltip>
            </Grid>
            <Grid item xs="auto">
                <Tooltip title="Zero throttle">
                    <IconButton disabled={status.isLoading} onClick={() => update({ throttle: 0.0 }) }>
                        <GiStopSign size={iconSize} />
                    </IconButton>
                </Tooltip>
            </Grid>
            <Grid item xs="auto">
                <Tooltip title="Center Aux">
                <IconButton disabled={status.isLoading} onClick={() => update({ aux_x: 0.0, aux_y: 0.0 }) }>
                        <GiJoystick size={iconSize} />
                    </IconButton>
                </Tooltip>
            </Grid>
            <Grid item xs={12}>
                <Stack>
                    <AxisSlider title="Steering" axis="steering" />
                    <AxisSlider title="Throttle" axis="throttle" />
                    <AxisSlider title="Aux X" axis="aux_x" />
                    <AxisSlider title="Aux Y" axis="aux_y" />
                </Stack>
            </Grid>
        </Grid>
   )
}