import { Box, FormLabel, Grid, Slider, Typography } from "@mui/material";
import LightModeIcon from '@mui/icons-material/LightMode';
import LightModeOutlinedIcon from '@mui/icons-material/LightModeOutlined';
import { useGetLEDSQuery, useSetLEDSMutation } from "../../services/leds";
import { useState } from "react";
import { useGetInputQuery } from "../../services/input";
import { InputSource } from "../../services/model";



interface Props {
    disabled?: boolean,
}


export default function BrightnessSlider({ disabled = false }: Props) {
    const { data: leds, isSuccess, isError } = useGetLEDSQuery()
    const { data: input } = useGetInputQuery();
    const [ update ] = useSetLEDSMutation()

    const [brightness, setBrightness ] = useState<number>(1.0);
    const [serverBrightness, setServerBrightness ] = useState<number>(leds?.brightness||1.0);

    if (isSuccess && leds!==undefined && serverBrightness!==leds.brightness) {
        setServerBrightness(leds.brightness);
        setBrightness(leds.brightness);
    }

    const handleChange = (event: Event, newValue: number|number[]) => {
        const value = newValue as number;
        setBrightness(value);
    };
    const handleChangeCommit = (event: any, newValue: number|number[]) => {
        const value = newValue as number;
        update({ brightness: value })
            .unwrap()
            .then(payload => {
                setBrightness(payload.brightness);
            })
    }

    const isDisabled = disabled || !isSuccess || input?.led_source !== InputSource.WEB;

    return (
        <Box>
            <FormLabel>
                <Typography id="brightness-slider" gutterBottom color={isError?"error":undefined} variant="caption">
                    Brightness
                </Typography>
            </FormLabel>
            <Grid container spacing={2} alignItems="center">
                <Grid item>
                    <LightModeOutlinedIcon color={isDisabled?"disabled":undefined} />
                </Grid>
                <Grid item xs>
                    <Slider
                        value={brightness}
                        disabled={isDisabled}
                        step={0.01}
                        min={0.0}
                        max={1.0}
                        onChange={handleChange}
                        onChangeCommitted={handleChangeCommit}
                    />
                </Grid>
                <Grid item>
                    <LightModeIcon color={isDisabled?"disabled":undefined} />
                </Grid>
            </Grid>
        </Box>
    )
}