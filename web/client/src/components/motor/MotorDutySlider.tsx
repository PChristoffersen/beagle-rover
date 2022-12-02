import { Box, Grid } from "@mui/material";
import { useGetMotorQuery, useSetMotorMutation } from "../../services/motors";
import { useState } from "react";
import CenterSlider from "../common/CenterSlider";

interface Props {
    id: number
    disabled?: boolean
}


function dutyText(value: number|undefined): string {
    if (value === undefined) {
        return ""
    }
    return (value*100).toLocaleString(undefined, { maximumFractionDigits: 0 })+"%"
}



export default function MotorDutySlider({ id, disabled = false }: Props) {
    const { data: motor, isSuccess } = useGetMotorQuery(id)
    const [ update ] = useSetMotorMutation();

    const [duty, setDuty ] = useState<number>(0.0);
    const [serverDuty, setServerDuty ] = useState<number>(motor?.duty||0.0);

    if (isSuccess && motor!==undefined && serverDuty!==motor.duty) {
        setServerDuty(motor.duty)
        setDuty(motor.duty);
    }

    const motorEnabled = !disabled && (motor?.enabled || false);
    const limitMin = -1.0;
    const limitMax =  1.0;
    const step = 0.01;

    const isDisabled = disabled || !isSuccess || !motorEnabled;

    const marks = [
        {
            value: limitMin,
            label: dutyText(limitMin)
        },
        {
            value: 0.0,
            label: dutyText(0.0)
        },
        {
            value: limitMax,
            label: dutyText(limitMax)
        },
    ]

    const handleChange = (event: Event, newValue: number|number[]) => {
        const value = newValue as number;
        setDuty(value);
    };
    const handleChangeCommit = (event: any, newValue: number|number[]) => {
        const value = newValue as number;
        update({ id: id, duty: value })
            .unwrap()
            .then(payload => {
                setDuty(payload.duty);
            });
    }

    return (
        <Box>
            <Grid container spacing={0} alignItems="center">
                <Grid item xs sx={{ paddingLeft: 2, paddingRight: 2 }}>
                    <CenterSlider
                        value={duty}
                        disabled={isDisabled}
                        step={step}
                        min={limitMin} 
                        max={limitMax} 
                        valueLabelFormat={dutyText}
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