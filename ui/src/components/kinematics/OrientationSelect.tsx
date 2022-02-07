import * as React from 'react';
import { FormControl, InputLabel, ListItemIcon, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { Orientation, orientations, useGetKinematicQuery, useSetKinematicMutation } from '../../services/kinematic';
import ArrowUpwardIcon from '@mui/icons-material/ArrowUpward';
import ArrowDownwardIcon from '@mui/icons-material/ArrowDownward';
import ArrowForwardIcon from '@mui/icons-material/ArrowForward';
import ArrowBackIcon from '@mui/icons-material/ArrowBack';



export default function OrientationSelect() {
    const { data: kinematic, error, isLoading } = useGetKinematicQuery()
    const [ update ] = useSetKinematicMutation();

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as Orientation;
        update({ orientation: value });
    };

    const isReady = !error && !isLoading;
    const isError = !!error
    const value = kinematic?.orientation || "unk";

    return (
        <FormControl fullWidth error={isError} >
            <InputLabel id="orientation-select-label">Orientation</InputLabel>
            <Select
                labelId="orientation-select-label"
                id="orientation-select"
                value={value}
                label="Orientation"
                onChange={handleChange}
                disabled={!isReady}
                renderValue={(selected) => {
                    if (selected.length === 0) {
                        return <em>Placeholder</em>;
                    }
                    return selected.charAt(0).toUpperCase()+selected.slice(1).toLowerCase();
                }}
            >
                { isReady && [
                    <MenuItem key={Orientation.NORTH} value={Orientation.NORTH}>
                        <ListItemIcon><ArrowUpwardIcon /></ListItemIcon>
                        North
                    </MenuItem>,
                    <MenuItem key={Orientation.WEST} value={Orientation.WEST}>
                        <ListItemIcon><ArrowForwardIcon /></ListItemIcon>
                        West
                    </MenuItem>,
                    <MenuItem key={Orientation.EAST} value={Orientation.EAST}>
                        <ListItemIcon><ArrowBackIcon /></ListItemIcon>
                        East
                    </MenuItem>,
                    <MenuItem key={Orientation.SOUTH} value={Orientation.SOUTH}>
                        <ListItemIcon><ArrowDownwardIcon /></ListItemIcon>
                        South
                    </MenuItem>
                ]}
                { !isReady && <MenuItem disabled key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}
