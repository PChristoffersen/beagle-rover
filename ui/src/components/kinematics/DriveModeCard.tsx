import * as React from 'react';
import { Card, CardHeader, CardContent, Box, CircularProgress, RadioGroup, FormControlLabel, Radio } from '@mui/material';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

import { DriveMode, driveModes, useGetKinematicQuery, useSetKinematicMutation } from '../../services/kinematic';




export default function DriveModeCard() {
    const { data: kinematic, isError, isLoading, isSuccess } = useGetKinematicQuery()
    const [ update ] = useSetKinematicMutation();

    const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        const value = event.target.value as DriveMode
        update({ drive_mode: value })
    }

    return (
        <Card sx={{ minWidth: 275 }}>
            <CardHeader title="Drive Mode" />
            <CardContent>
                { !isSuccess && 
                    <Box display="flex" justifyContent="center" alignItems="center">
                        { isSuccess && <ErrorOutlineIcon color="error" fontSize="large" /> }
                        { !isSuccess && isLoading && <CircularProgress /> }
                    </Box>
                }
                { isSuccess &&
                    <RadioGroup 
                        name="kinematic-drive-mode"
                        value={kinematic?.drive_mode}
                        onChange={handleChange}
                    >
                        {driveModes.map(entry => (
                            <FormControlLabel key={entry.key} value={entry.key} control={<Radio />} label={entry.name} disabled={entry.disabled} />
                        ))}
                    </RadioGroup>
                }
            </CardContent>
        </Card>
    )

}