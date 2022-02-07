import { FormControl, InputLabel, MenuItem, Select, SelectChangeEvent } from "@mui/material";
import { IndicatorMode, indicatorModes, useGetLEDSQuery, useSetLEDSMutation } from '../../services/leds';



export default function IndicatorModeSelect() {
    const { data: leds, error, isLoading } = useGetLEDSQuery()
    const [ update ] = useSetLEDSMutation()

    const handleChange = (event: SelectChangeEvent) => {
        const value = event.target.value as IndicatorMode;
        update({ indicators: value });
    };

    const isReady = !error && !isLoading;
    const isError = !!error
    const value = leds?.indicators || "unk";

    return (
        <FormControl fullWidth error={isError} >
            <InputLabel id="indicator-mode-select-label">Indicators</InputLabel>
            <Select
                labelId="indicator-mode-select-label"
                id="indicator-mode-select"
                value={value}
                label="Indicators"
                onChange={handleChange}
                disabled={!isReady}
            >
                { isReady && indicatorModes.map(entry => (
                    <MenuItem key={entry.key} value={entry.key} disabled={entry.disabled}>{entry.name}</MenuItem>
                ))}
                { !isReady && <MenuItem key="unk" value="unk" >Unknown</MenuItem>}
            </Select>
        </FormControl>
    )
}
