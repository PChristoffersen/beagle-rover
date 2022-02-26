import * as React from 'react';
import { FormControl, InputLabel, ListItemIcon, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { Orientation, orientationName, useGetKinematicQuery, useSetKinematicMutation } from '../../services/kinematic';
import ArrowUpwardIcon from '@mui/icons-material/ArrowUpward';
import ArrowDownwardIcon from '@mui/icons-material/ArrowDownward';
import ArrowForwardIcon from '@mui/icons-material/ArrowForward';
import ArrowBackIcon from '@mui/icons-material/ArrowBack';





export default function OrientationSelect() {
    const { data: kinematic, isError, isSuccess } = useGetKinematicQuery()
    const [ update ] = useSetKinematicMutation();

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as Orientation;
        update({ orientation: value });
    };

    const value = kinematic?.orientation || "unk";

    return (
        <FormControl fullWidth variant="standard" error={isError} >
            <InputLabel id="orientation-select-label">Orientation</InputLabel>
            <Select
                labelId="orientation-select-label"
                id="orientation-select"
                value={value}
                label="Orientation"
                onChange={handleChange}
                disabled={!isSuccess}
                renderValue={(selected) => {
                    if (selected.length === 0) {
                        return <em>Placeholder</em>;
                    }
                    return orientationName(selected as Orientation)
                }}
            >
                { isSuccess && [
                    <MenuItem key={Orientation.NORTH} value={Orientation.NORTH}>
                        <ListItemIcon><ArrowUpwardIcon /></ListItemIcon>
                        {orientationName(Orientation.NORTH)}
                    </MenuItem>,
                    <MenuItem key={Orientation.WEST} value={Orientation.WEST}>
                        <ListItemIcon><ArrowBackIcon /></ListItemIcon>
                        {orientationName(Orientation.WEST)}
                    </MenuItem>,
                    <MenuItem key={Orientation.EAST} value={Orientation.EAST}>
                        <ListItemIcon><ArrowForwardIcon /></ListItemIcon>
                        {orientationName(Orientation.EAST)}
                    </MenuItem>,
                    <MenuItem key={Orientation.SOUTH} value={Orientation.SOUTH}>
                        <ListItemIcon><ArrowDownwardIcon /></ListItemIcon>
                        {orientationName(Orientation.SOUTH)}
                    </MenuItem>
                ]}
                { !isSuccess && <MenuItem disabled key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}
