import * as React from 'react';
import { FormControl, InputLabel, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { DriveMode, driveModes, useGetKinematicQuery, useSetKinematicMutation } from '../../services/kinematic';



export default function DriveModeSelect() {
    const { data: kinematic, error, isLoading } = useGetKinematicQuery()
    const [ update ] = useSetKinematicMutation();

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as DriveMode;
        update({ drive_mode: value });
    };

    const isReady = !error && !isLoading;
    const isError = !!error
    const value = kinematic?.drive_mode || "unk";

    return (
        <FormControl fullWidth error={isError} >
            <InputLabel id="drive-mode-select-label">Drive mode</InputLabel>
            <Select
                labelId="drive-mode-select-label"
                id="drive-mode-select"
                value={value}
                label="Drive mode"
                onChange={handleChange}
                disabled={!isReady}
            >
                { isReady && driveModes.map(entry => (
                    <MenuItem key={entry.key} value={entry.key} disabled={entry.disabled}>{entry.name}</MenuItem>
                ))}
                { !isReady && <MenuItem key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}
