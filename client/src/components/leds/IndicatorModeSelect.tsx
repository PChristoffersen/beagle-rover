import { FormControl, InputLabel, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { InputSource, useGetInputQuery } from "../../services/input";
import { IndicatorMode, indicatorModes, useGetLEDSQuery, useSetLEDSMutation } from '../../services/leds';



interface Props {
    disabled?: boolean
}

export default function IndicatorModeSelect({ disabled }: Props) {
    const { data: leds, isSuccess, isError } = useGetLEDSQuery();
    const { data: input } = useGetInputQuery();
    const [ update ] = useSetLEDSMutation();

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as IndicatorMode;
        update({ indicators: value });
    };

    const value = leds?.indicators || "unk";
    const isDisabled = disabled || !isSuccess || input?.led_source !== InputSource.WEB;

    return (
        <FormControl fullWidth variant="standard" error={isError} >
            <InputLabel id="indicator-mode-select-label">Indicators</InputLabel>
            <Select
                labelId="indicator-mode-select-label"
                id="indicator-mode-select"
                value={value}
                label="Indicators"
                onChange={handleChange}
                disabled={isDisabled}
            >
                { isSuccess && indicatorModes.map(entry => (
                    <MenuItem key={entry.key} value={entry.key} disabled={entry.disabled}>{entry.name}</MenuItem>
                ))}
                { !isSuccess && <MenuItem key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}
