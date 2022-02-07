import * as React from 'react';
import { Card, CardHeader, CardContent, Box, CircularProgress, RadioGroup, FormControlLabel, Radio } from '@mui/material';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

import { useGetLEDSQuery, useSetLEDSMutation, IndicatorMode, indicatorModes } from "../../services/leds";




export default function LEDIndicatorCard() {
    const { data: leds, error, isLoading } = useGetLEDSQuery()
    const [ updateLEDS ] = useSetLEDSMutation();

    const isReady = !error && !isLoading

    const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        const value = event.target.value as IndicatorMode
        updateLEDS({ indicators: value })
    }

    return (
        <Card sx={{ minWidth: 275 }}>
            <CardHeader title="Indicators" />
            <CardContent>
                { !isReady && 
                    <Box display="flex" justifyContent="center" alignItems="center">
                        { error && <ErrorOutlineIcon color="error" fontSize="large" /> }
                        { !error && isLoading && <CircularProgress /> }
                    </Box>
                }
                { isReady &&
                    <RadioGroup 
                        name="led-indicator-mode"
                        value={leds?.indicators}
                        onChange={handleChange}
                    >
                        {indicatorModes.map(entry => (
                            <FormControlLabel key={entry.key} value={entry.key} control={<Radio />} label={entry.name} />
                        ))}
                    </RadioGroup>
                }
            </CardContent>
        </Card>
    )

}