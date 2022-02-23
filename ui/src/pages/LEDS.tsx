import { Card, CardContent, CardHeader, Grid, Stack } from '@mui/material';
import Box from '@mui/material/Box';
import AnimationModeCard from '../components/leds/AnimationModeCard';
import AnimationModeSelect from '../components/leds/AnimationModeSelect';
import BackgroundPicker from '../components/leds/BackgroundPicker';
import BrightnessSlider from '../components/leds/BrightnessSlider';
import IndicatorModeCard from '../components/leds/IndicatorModeCard';
import IndicatorModeSelect from '../components/leds/IndicatorModeSelect';
import OutputCard from '../components/leds/OutputCard';

import Page from '../components/Page';


function SettingsCard() {
    return (
        <Card>
            <CardHeader title="Settings" />
            <CardContent>
                <Stack spacing={3}>
                    <AnimationModeSelect />
                    <IndicatorModeSelect />
                    <BackgroundPicker />
                    <BrightnessSlider />
                </Stack>
            </CardContent>
        </Card>
    )
}


export default function LEDS() {
    return (
        <Page>
            <Grid container spacing={{ xs: 1, sm: 2 }} sx={{ padding: { xs: 1, sm: 2 } }}>
                <Grid item xs={12} sm="auto">
                    <SettingsCard />
                </Grid>
                <Grid item xs={12} sm>
                    <OutputCard />
                </Grid>
            </Grid>
        </Page>
    )
}