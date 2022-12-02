import { Box, FormLabel, Typography } from "@mui/material";
import { useState } from "react";
import { useGetInputQuery, useGetInputStateQuery, useSetInputStateMutation } from "../../services/input";
import { InputSource } from "../../services/model";
import CenterSlider from "../common/CenterSlider";


export interface AxisSliderProps {
    disabled?: boolean;
    title?: string;
    axis: "direction" | "throttle" | "aux_x" | "aux_y";
}

export default function AxisSlider({ disabled, axis, title }: AxisSliderProps) {
    const { data: input } = useGetInputQuery();
    const { data, isSuccess, isError } = useGetInputStateQuery();
    const [ update ] = useSetInputStateMutation();

    const [value, setValue ] = useState<number>(0.0);
    const [serverValue, setServerValue ] = useState<number>(data?.[axis]||0.0);

    const isDisabled = disabled || !isSuccess || input?.axis_source !== InputSource.WEB;

    if (isSuccess && data!==undefined && serverValue!==data?.[axis]) {
        setServerValue(data?.[axis]);
        setValue(data?.[axis]);
    }

    const handleChange = (event: Event, newValue: number|number[]) => {
        const value = newValue as number;
        setValue(value);
    };
    const handleChangeCommit = (event: any, newValue: number|number[]) => {
        const value = newValue as number;
        update({ [axis]: value })
            .unwrap()
            .then(payload => {
                setValue(payload?.[axis]||0.0);
            })
    }

    const marks = [
        {
            value: -1.0,
            label: -1.0,
        },
        {
            value: -0.5,
        },
        {
            value: 0.0,
            label: 0.0,
        },
        {
            value: 0.5,
        },
        {
            value: 1.0,
            label: 1.0,
        },
    ]

    return (
        <Box>
            { title &&
                <FormLabel>
                    <Typography id={axis+"-slider"} gutterBottom color={isError?"error":undefined} variant="caption">
                        {title}
                    </Typography>
                </FormLabel>
            }
            <CenterSlider
                value={value}
                disabled={isDisabled}
                step={0.01}
                min={-1.0}
                max={1.0}
                marks={marks}
                onChange={handleChange}
                onChangeCommitted={handleChangeCommit}
            />
        </Box>
    )
}
