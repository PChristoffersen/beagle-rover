import * as React from 'react';
import { FormControl, InputLabel, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { useGetInputQuery, useGetInputSourcesQuery, useSetInputMutation } from '../../services/input';
import { InputSource, InputSourceDescription } from '../../services/model';


interface Props {
    title?: string;
    source: "axis_source" | "kinematic_source" | "led_source";
}

export default function InputSourceSelect({ title, source }: Props) {
    const { data: inputSources } = useGetInputSourcesQuery();
    const { data: input, isError, isSuccess } = useGetInputQuery();
    const [ update ] = useSetInputMutation();

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as InputSource;
        update({ [source]: value });
    };

    const value = input?.[source] || "unk";
    const labels = {
        axis_source: "Axis control",
        kinematic_source: "Kinematic control",
        led_source: "LEDS control",
    }

    //console.log("Input", input)

    return (
        <FormControl fullWidth variant="standard" error={isError} >
            <InputLabel id="input-source-select-label">{title || labels[source]}</InputLabel>
            <Select
                labelId="input-source-select-label"
                id="input-source-select"
                value={value}
                label={title || labels[source]}
                onChange={handleChange}
                disabled={!isSuccess}
            >
                { isSuccess && inputSources && inputSources.map((entry: InputSourceDescription) => (
                    <MenuItem key={entry.key} value={entry.key} disabled={entry.disabled}>{entry.name}</MenuItem>
                ))}
                { !isSuccess && <MenuItem key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}