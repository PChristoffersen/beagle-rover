import Box from '@mui/material/Box';
import LEDAnimationCard from '../components/leds/LEDAnimationCard';
import LEDIndicatorCard from '../components/leds/LEDIndicatorCard';

import Page from '../components/Page';

export default function LEDS() {
    return (
        <Page>
            <Box
                sx={{
                    display: 'flex',
                    flexWrap: 'wrap',
                    '& > :not(style)': {
                    m: 1,
                    width: 128,
                    },
                }}
            >
                <LEDAnimationCard />
                <LEDIndicatorCard />
            </Box>
        </Page>
    )
}