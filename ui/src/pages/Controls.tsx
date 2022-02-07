import { Card, CardContent, CardHeader, Stack } from '@mui/material';
import Box from '@mui/material/Box';
import InputSourceCard from '../components/input/InputSourceCard';
import InputSourceSelect from '../components/input/InputSourceSelect';
import DriveModeCard from '../components/kinematics/DriveModeCard';
import DriveModeSelect from '../components/kinematics/DriveModeSelect';
import OrientationCard from '../components/kinematics/OrientationCard';
import OrientationSelect from '../components/kinematics/OrientationSelect';

import Page from '../components/Page';


function SettingsCard() {
    return (
        <Card>
            <CardHeader title="Settings" />
            <CardContent>
                <Stack spacing={3}>
                    <InputSourceSelect />
                    <DriveModeSelect />
                    <OrientationSelect />
                </Stack>
            </CardContent>
        </Card>
    )
}


export default function Controls() {
    return (
        <Page>
            <Box
                sx={{
                    display: 'flex',
                    flexWrap: 'wrap',
                    '& > :not(style)': {
                    m: 1,
                    width: 'fit-content',
                    minWidth: 300,
                    },
                }}
            >
                <SettingsCard />
                <InputSourceCard />
                <DriveModeCard />
                <OrientationCard />
            </Box>
        </Page>
    )
}