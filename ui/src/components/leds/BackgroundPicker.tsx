import { useEffect, useState, KeyboardEvent } from "react";
import { Color, ColorError, ColorPicker, ColorValue, createColor } from "@dmitrychebayewski/mui-color";
import { Box, FormControl } from "@mui/material";
import { useGetLEDSQuery, useSetLEDSMutation } from "../../services/leds";


interface Props {
    disabled?: boolean,
}

export default function BackgroundPicker({ disabled=false }: Props) {
    const { data: leds, isSuccess, isError } = useGetLEDSQuery()
    const [ update ] = useSetLEDSMutation()

    const [ serverColor, setServerColor ] = useState<string>(leds?.background || "#000000")
    const [ color, setColor ] = useState<ColorValue>(createColor("#000000"))
    const [ isOpen, setOpen ] = useState<boolean>(false);
    const [ error, setError ] = useState<string|undefined>(undefined)


    if (isSuccess && leds!=undefined && serverColor!==leds.background) {
        setServerColor(leds.background);
        setColor(createColor("#"+leds.background));
    }

    const handleChange = (newValue: ColorValue) => {
        const valueType = typeof newValue;

        let value = newValue as Color;
        if (valueType === "string" || valueType === "number") {
            value = createColor(newValue)
        }

        if ((value as ColorError).error) {
            setError((value as ColorError).error);
        }
        else if ((value as Color).format === "unknown") {
            setError("Unknown format");
        }
        else {
            setError(undefined);
        }


        setColor(newValue)
    }
    const handleChangeCommit = () => {
        let value = color;
        const valueType = typeof value;

        if (valueType === "string" || valueType === "number") {
            value = createColor(value)
        }
        if ((value as ColorError).error || !(value as Color).format || (value as Color).format === "unknown") {
            return;
        }
        
        if ((value as Color).hex) {
            const col = value as Color;
            if (col.hex!==leds?.background) {
                setServerColor(col.hex);
                update({ background: col.hex });
            }
        }
    }
    const handleKeyPress = (event: KeyboardEvent<HTMLDivElement>) => {
        if (event.key === "Enter") {
            handleChangeCommit();
        }
    }
    const handleOpen = (open: boolean) => {
        if (!open) {
            handleChangeCommit();
        }
        setOpen(open);
    }

    useEffect(() => {
        if (isOpen) {
            const timeOutId = setTimeout(handleChangeCommit, 200)
            return () => clearTimeout(timeOutId);
        }
        else {
            return () => {}
        }
    }, [color]);

    const isDisabled = disabled || !isSuccess;

    return (
        <Box>
            <ColorPicker 
                variant="standard"
                label="Background"
                error={isError || Boolean(error)}
                helperText={error}
                disabled={isDisabled}
                value={color}
                disableAlpha={true}
                onChange={handleChange}
                onOpen={handleOpen}
                onKeyPress={handleKeyPress}
            />
        </Box>
    )
}