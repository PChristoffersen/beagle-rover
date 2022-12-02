import * as React from 'react';
import { FormControl, InputLabel, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { useGetDriveModesQuery, useGetKinematicQuery, useSetKinematicMutation } from '../../services/kinematic';
import { DriveMode, DriveModeDescription } from '../../services/model';



export default function DriveModeSelect() {
    const { data: driveModes, isSuccess: isDriveModesSuccess } = useGetDriveModesQuery();
    const { data: kinematic, isError, isSuccess } = useGetKinematicQuery();
    const [ update ] = useSetKinematicMutation();

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as DriveMode;
        update({ drive_mode: value });
    };

    const isAllSuccess = isSuccess && isDriveModesSuccess;
    const value = (isAllSuccess && kinematic?.drive_mode) || "unk";

    return (
        <FormControl fullWidth variant="standard" error={isError} >
            <InputLabel id="drive-mode-select-label">Drive mode</InputLabel>
            <Select
                labelId="drive-mode-select-label"
                id="drive-mode-select"
                value={value}
                label="Drive mode"
                onChange={handleChange}
                disabled={!isAllSuccess}
            >
                { isAllSuccess && driveModes && driveModes.map((entry: DriveModeDescription) => (
                    <MenuItem key={entry.key} value={entry.key} disabled={entry.disabled}>{entry.name}</MenuItem>
                ))}
                { !isAllSuccess && <MenuItem key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}
