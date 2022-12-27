import * as React from 'react';
import { FormControl, InputLabel, ListItemIcon, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { useGetKinematicQuery, useGetOrientationsQuery, useSetKinematicMutation } from '../../services/kinematic';
import ArrowUpwardIcon from '@mui/icons-material/ArrowUpward';
import ArrowDownwardIcon from '@mui/icons-material/ArrowDownward';
import ArrowForwardIcon from '@mui/icons-material/ArrowForward';
import ArrowBackIcon from '@mui/icons-material/ArrowBack';
import { InputSource, Orientation, OrientationDescription, OrientationDescriptionList } from '../../services/model';
import { useGetInputQuery } from '../../services/input';




export function orientationName(list: OrientationDescriptionList|undefined, orientation: Orientation): string|undefined {
    if (list) {
        return list.find((value: OrientationDescription) => value.key===orientation)?.name;
    }
    return "unk";
}


export default function OrientationSelect() {
    const { data: orientations } = useGetOrientationsQuery();
    const { data: kinematic, isError, isSuccess } = useGetKinematicQuery();
    const { data: input } = useGetInputQuery();
    const [ update ] = useSetKinematicMutation();

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as Orientation;
        update({ orientation: value });
    };

    const value = kinematic?.orientation || "unk";

    const isDisabled = !isSuccess || input?.kinematic_source !== InputSource.WEB;

    return (
        <FormControl fullWidth variant="standard" error={isError} >
            <InputLabel id="orientation-select-label">Orientation</InputLabel>
            <Select
                labelId="orientation-select-label"
                id="orientation-select"
                value={value}
                label="Orientation"
                onChange={handleChange}
                disabled={isDisabled}
                renderValue={(selected) => {
                    if (selected.length === 0) {
                        return <em>Placeholder</em>;
                    }
                    return orientationName(orientations, selected as Orientation)
                }}
            >
                { isSuccess && [
                    <MenuItem key={Orientation.NORTH} value={Orientation.NORTH}>
                        <ListItemIcon><ArrowUpwardIcon /></ListItemIcon>
                        {orientationName(orientations, Orientation.NORTH)}
                    </MenuItem>,
                    <MenuItem key={Orientation.WEST} value={Orientation.WEST}>
                        <ListItemIcon><ArrowBackIcon /></ListItemIcon>
                        {orientationName(orientations, Orientation.WEST)}
                    </MenuItem>,
                    <MenuItem key={Orientation.EAST} value={Orientation.EAST}>
                        <ListItemIcon><ArrowForwardIcon /></ListItemIcon>
                        {orientationName(orientations, Orientation.EAST)}
                    </MenuItem>,
                    <MenuItem key={Orientation.SOUTH} value={Orientation.SOUTH}>
                        <ListItemIcon><ArrowDownwardIcon /></ListItemIcon>
                        {orientationName(orientations, Orientation.SOUTH)}
                    </MenuItem>
                ]}
                { !isSuccess && <MenuItem disabled key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}
