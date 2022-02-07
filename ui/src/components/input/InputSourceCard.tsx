import * as React from 'react';
import { Card, CardHeader, CardContent, Box, CircularProgress, RadioGroup, FormControlLabel, Radio } from '@mui/material';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

import { InputSource, inputSources, useGetInputQuery, useSetInputMutation } from '../../services/input';




export default function InputSourceCard() {
    const { data: input, error, isLoading } = useGetInputQuery()
    const [ update ] = useSetInputMutation();

    const isReady = !error && !isLoading

    const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        const value = event.target.value as InputSource
        update({ source: value })
    }

    return (
        <Card sx={{ minWidth: 275 }}>
            <CardHeader title="Input source" />
            <CardContent>
                { !isReady && 
                    <Box display="flex" justifyContent="center" alignItems="center">
                        { error && <ErrorOutlineIcon color="error" fontSize="large" /> }
                        { !error && isLoading && <CircularProgress /> }
                    </Box>
                }
                { isReady &&
                    <RadioGroup 
                        name="input-source"
                        value={input?.source}
                        onChange={handleChange}
                    >
                        {inputSources.map(entry => (
                            <FormControlLabel key={entry.key} value={entry.key} control={<Radio />} label={entry.name} />
                        ))}
                    </RadioGroup>
                }
            </CardContent>
        </Card>
    )

}