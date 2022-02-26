import * as React from 'react';
import { Card, CardHeader, CardContent, Box, CircularProgress, RadioGroup, FormControlLabel, Radio } from '@mui/material';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

import { InputSource, inputSources, useGetInputQuery, useSetInputMutation } from '../../services/input';


interface Props {
    source: "axis_source" | "kinematic_source" | "led_source";
}


export default function InputSourceCard({ source }: Props) {
    const { data: input, isError, isLoading, isSuccess } = useGetInputQuery()
    const [ update ] = useSetInputMutation();

    const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        const value = event.target.value as InputSource
        update({ [source]: value })
    }

    return (
        <Card sx={{ minWidth: 275 }}>
            <CardHeader title="Input source" />
            <CardContent>
                { !isSuccess && 
                    <Box display="flex" justifyContent="center" alignItems="center">
                        { isError && <ErrorOutlineIcon color="error" fontSize="large" /> }
                        { !isError && isLoading && <CircularProgress /> }
                    </Box>
                }
                { isSuccess &&
                    <RadioGroup 
                        name="input-source"
                        value={input?.[source]}
                        onChange={handleChange}
                    >
                        {inputSources.map(entry => (
                            <FormControlLabel key={entry.key} value={entry.key} control={<Radio />} label={entry.name} disabled={entry.disabled} />
                        ))}
                    </RadioGroup>
                }
            </CardContent>
        </Card>
    )

}