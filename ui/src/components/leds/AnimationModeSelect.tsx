import { FormControl, InputLabel, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { InputSource, useGetInputQuery } from "../../services/input";
import { AnimationMode, animationModes, useGetLEDSQuery, useSetLEDSMutation } from '../../services/leds';


interface Props {
    disabled?: boolean
}


export default function AnimationModeSelect({ disabled }: Props) {
    const { data: leds, isSuccess, isError } = useGetLEDSQuery()
    const { data: input } = useGetInputQuery();
    const [ update ] = useSetLEDSMutation()

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as AnimationMode;
        update({ animation: value });
    };

    const value = leds?.animation || "unk";
    const isDisabled = disabled || !isSuccess || input?.led_source !== InputSource.WEB;

    return (
        <FormControl fullWidth variant="standard" error={isError} >
            <InputLabel id="animation-mode-select-label">Animation</InputLabel>
            <Select
                labelId="animation-mode-select-label"
                id="animation-mode-select"
                value={value}
                label="Animation"
                onChange={handleChange}
                disabled={isDisabled}
            >
                { isSuccess && animationModes.map(entry => (
                    <MenuItem key={entry.key} value={entry.key} disabled={entry.disabled}>{entry.name}</MenuItem>
                ))}
                { !isSuccess && <MenuItem key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}
