import { Box, Grid, IconButton, Slider } from "@mui/material";
import StopCircleIcon from '@mui/icons-material/StopCircle';
import ChevronLeftIcon from '@mui/icons-material/ChevronLeft';
import ChevronRightIcon from '@mui/icons-material/ChevronRight';
import { useGetMotorQuery, useSetMotorMutation } from "../../services/motors";
import { useState } from "react";

interface Props {
    id: number
    disabled?: boolean
}


function angleText(value: number|undefined): string {
    if (value === undefined) {
        return ""
    }
    return (value * 180.0 / Math.PI).toLocaleString(undefined, { minimumFractionDigits: 1, maximumFractionDigits: 1 }) + "°"
}


export default function ServoAngleSlider({ id, disabled=false }: Props) {
    const { data: motor, isSuccess } = useGetMotorQuery(id)
    const [ update ] = useSetMotorMutation();
    const servo = motor?.servo;

    const [angle, setAngle ] = useState<number>(0.0);
    const [serverAngle, setServerAngle ] = useState<number>(servo?.angle||0.0);

    if (isSuccess && servo!=undefined && serverAngle!==servo.angle) {
        setServerAngle(servo.angle)
        setAngle(servo.angle);
    }

    const servoEnabled = !disabled && (servo?.enabled || false);
    const limitMin = servo?.limit_min || -Math.PI/2.0;
    const limitMax = servo?.limit_max || Math.PI/2.0;
    const step = (limitMax-limitMin)/180.0;

    const isDisabled = disabled || !isSuccess || !servoEnabled;

    const marks = [
        {
            value: limitMin,
            label: angleText(limitMin)
        },
        {
            value: 0.0,
            label: angleText(0.0)
        },
        {
            value: limitMax,
            label: angleText(limitMax)
        },
    ]


    const handleChange = (event: Event, newValue: number|number[]) => {
        const value = newValue as number;
        setAngle(value);
    };
    const handleChangeCommit = (event: any, newValue: number|number[]) => {
        const value = newValue as number;
        update({ id: id, servo: { angle: value } })
            .unwrap()
            .then(payload => {
                if (payload.servo?.angle!=undefined) {
                    setAngle(payload.servo.angle);
                }
            });
    }

    return (
        <Box>
            <Grid container spacing={0} alignItems="center">
                <Grid item xs sx={{ paddingLeft: 2, paddingRight: 2 }}>
                    <Slider
                        value={angle}
                        disabled={isDisabled}
                        step={step}
                        min={limitMin} 
                        max={limitMax} 
                        valueLabelFormat={angleText}
                        valueLabelDisplay='auto'
                        marks={marks}
                        onChange={handleChange}
                        onChangeCommitted={handleChangeCommit} 

                    />
                </Grid>
            </Grid>
        </Box>
    )
}