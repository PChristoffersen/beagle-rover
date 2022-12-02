import { Card, CardContent, CardHeader, Grid, Stack } from '@mui/material';
import InputSourceSelect from '../components/input/InputSourceSelect';
import AnimationModeSelect from '../components/leds/AnimationModeSelect';
import BackgroundPicker from '../components/leds/BackgroundPicker';
import BrightnessSlider from '../components/leds/BrightnessSlider';
import IndicatorModeSelect from '../components/leds/IndicatorModeSelect';
import OutputCard from '../components/leds/OutputCard';

import Page from '../components/Page';


function SettingsCard() {
    return (
        <Card>
            <CardHeader title="Settings" />
            <CardContent>
                <Stack spacing={3}>
                    <InputSourceSelect title="Controller" source="led_source" />
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
            <Grid container spacing={{ xs: 1, md: 2 }} sx={{ padding: { xs: 1, md: 2 } }}>
                <Grid item xs={12} order={{ xs: 2, md: 1 }} md="auto">
                    <SettingsCard />
                </Grid>
                <Grid item xs={12} order={{ xs: 1, md: 2 }} md>
                    <OutputCard />
                </Grid>
            </Grid>
        </Page>
    )
}