import { Box, Slider, SliderProps, Table, TableBody, TableCell, TableCellProps, TableRow } from "@mui/material";
import { styled } from '@mui/material/styles';
import { useState } from "react";
import { useGetInputStateQuery, useSetInputStateMutation } from "../../services/input";



function TableCellUnstyled(props: TableCellProps) {
    return (
        <TableCell {...props} sx={{ borderBottom: 0 }}>
            {props.children}
        </TableCell>
    )
}



interface AxisSliderProps {
    disabled?: boolean;
    axis: "steering" | "throttle" | "aux_x" | "aux_y";
}

function AxisSlider({ disabled, axis }: AxisSliderProps) {
    const { data, isSuccess } = useGetInputStateQuery();
    const [ update ] = useSetInputStateMutation();

    const [value, setValue ] = useState<number>(1.0);
    const [serverValue, setServerValue ] = useState<number>(data?.[axis]||0.0);

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

    return (
        <Slider 
            value={value}
            disabled={false}
            step={0.01}
            min={-1.0}
            max={1.0}
            onChange={handleChange}
            onChangeCommitted={handleChangeCommit}
        />
    )
}


export default function SliderSteering() {
    return (
        <Box>
            <Table >
                <TableBody>
                    <TableRow>
                        <TableCellUnstyled>Steering</TableCellUnstyled>
                        <TableCellUnstyled width="100%"><AxisSlider axis="steering" /></TableCellUnstyled>
                    </TableRow>
                    <TableRow>
                        <TableCellUnstyled>Throttle</TableCellUnstyled>
                        <TableCellUnstyled width="100%"><AxisSlider axis="throttle" /></TableCellUnstyled>
                    </TableRow>
                    <TableRow>
                        <TableCellUnstyled>Aux X</TableCellUnstyled>
                        <TableCellUnstyled width="100%"><AxisSlider axis="aux_x" /></TableCellUnstyled>
                    </TableRow>
                    <TableRow>
                        <TableCellUnstyled>Aux Y</TableCellUnstyled>
                        <TableCellUnstyled width="100%"><AxisSlider axis="aux_y" /></TableCellUnstyled>
                    </TableRow>
                </TableBody>
            </Table>

        </Box>
   )
}