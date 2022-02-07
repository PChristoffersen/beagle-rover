import { FormControl, InputLabel, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { AnimationMode, animationModes, useGetLEDSQuery, useSetLEDSMutation } from '../../services/leds';



export default function AnimationModeSelect() {
    const { data: leds, error, isLoading } = useGetLEDSQuery()
    const [ update ] = useSetLEDSMutation()

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as AnimationMode;
        update({ animation: value });
    };

    const isReady = !error && !isLoading;
    const isError = !!error
    const value = leds?.animation || "unk";

    return (
        <FormControl fullWidth error={isError} >
            <InputLabel id="animation-mode-select-label">Animation</InputLabel>
            <Select
                labelId="animation-mode-select-label"
                id="animation-mode-select"
                value={value}
                label="Animation"
                onChange={handleChange}
                disabled={!isReady}
            >
                { isReady && animationModes.map(entry => (
                    <MenuItem key={entry.key} value={entry.key} disabled={entry.disabled}>{entry.name}</MenuItem>
                ))}
                { !isReady && <MenuItem key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}
