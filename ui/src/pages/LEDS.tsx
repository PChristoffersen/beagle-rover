import { Card, CardContent, CardHeader, Stack } from '@mui/material';
import Box from '@mui/material/Box';
import AnimationModeCard from '../components/leds/AnimationModeCard';
import AnimationModeSelect from '../components/leds/AnimationModeSelect';
import IndicatorModeCard from '../components/leds/IndicatorModeCard';
import IndicatorModeSelect from '../components/leds/IndicatorModeSelect';

import Page from '../components/Page';


function SettingsCard() {
    return (
        <Card>
            <CardHeader title="Settings" />
            <CardContent>
                <Stack spacing={3}>
                    <AnimationModeSelect />
                    <IndicatorModeSelect />
                </Stack>
            </CardContent>
        </Card>
    )
}


export default function LEDS() {
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
                <AnimationModeCard />
                <IndicatorModeCard />
            </Box>
        </Page>
    )
}