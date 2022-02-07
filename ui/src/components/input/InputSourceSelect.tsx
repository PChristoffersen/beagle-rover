import * as React from 'react';
import { FormControl, InputLabel, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { InputSource, inputSources, useGetInputQuery, useSetInputMutation } from '../../services/input';



export default function InputSourceSelect() {
    const { data: input, error, isLoading } = useGetInputQuery()
    const [ update ] = useSetInputMutation();

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as InputSource;
        update({ source: value });
    };

    const isReady = !error && !isLoading;
    const isError = !!error
    const value = input?.source || "unk";

    return (
        <FormControl fullWidth error={isError} >
            <InputLabel id="input-source-select-label">Input source</InputLabel>
            <Select
                labelId="input-source-select-label"
                id="input-source-select"
                value={value}
                label="Input source"
                onChange={handleChange}
                disabled={!isReady}
            >
                { isReady && inputSources.map(entry => (
                    <MenuItem key={entry.key} value={entry.key} disabled={entry.disabled}>{entry.name}</MenuItem>
                ))}
                { !isReady && <MenuItem key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}