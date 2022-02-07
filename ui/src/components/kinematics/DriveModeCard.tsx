import * as React from 'react';
import { Card, CardHeader, CardContent, Box, CircularProgress, RadioGroup, FormControlLabel, Radio } from '@mui/material';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

import { DriveMode, driveModes, useGetKinematicQuery, useSetKinematicMutation } from '../../services/kinematic';




export default function DriveModeCard() {
    const { data: kinematic, error, isLoading } = useGetKinematicQuery()
    const [ update ] = useSetKinematicMutation();

    const isReady = !error && !isLoading

    const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        const value = event.target.value as DriveMode
        update({ drive_mode: value })
    }

    return (
        <Card sx={{ minWidth: 275 }}>
            <CardHeader title="Drive Mode" />
            <CardContent>
                { !isReady && 
                    <Box display="flex" justifyContent="center" alignItems="center">
                        { error && <ErrorOutlineIcon color="error" fontSize="large" /> }
                        { !error && isLoading && <CircularProgress /> }
                    </Box>
                }
                { isReady &&
                    <RadioGroup 
                        name="kinematic-drive-mode"
                        value={kinematic?.drive_mode}
                        onChange={handleChange}
                    >
                        {driveModes.map(entry => (
                            <FormControlLabel key={entry.key} value={entry.key} control={<Radio />} label={entry.name} />
                        ))}
                    </RadioGroup>
                }
            </CardContent>
        </Card>
    )

}