import { Slider, SliderProps } from "@mui/material";


/*
function CenterSliderThumb({ children, ...other }: any) {
    const dataIndex = other["data-index"];
    const ownerValue = other.ownerState.value as number[];
    console.log(dataIndex, ownerValue, other);

    var hidden = false;

    if (dataIndex===0 && ownerValue[0]===0) {
        hidden = true;
    }
    if (dataIndex===1 && ownerValue[1]===0 && ownerValue[0]!==0) {
        hidden = true;
    }


    if (hidden) {
        return (
            <Box display="hidden">
                {children}
            </Box>
        );
    }
    return (
        <SliderThumb  {...other}>
            {children}
        </SliderThumb>
    )
}
*/

interface CenterSliderProps extends SliderProps {
    baseValue?: number;
}


export default function CenterSlider({ track, baseValue=0.0, components, ...props }: CenterSliderProps) {
    return (
        <Slider
            track={false}
            {...props}
        />
    )
}

/*
export default function CenterSlider({ value, track, baseValue=0.0, onChange, onChangeCommitted, components, ...props }: CenterSliderProps) {
    const val = value as number;
    return (
        <Slider 
            {...props} 
            value={[
                val < baseValue ? val : baseValue,
                val > baseValue ? val : baseValue
            ]}
            components={{ Thumb: CenterSliderThumb }}
            onChange={(e, val, a) => {
                const value = val as number[];
                if (onChange) { 
                    const outVal = value[a];
                    console.log("OnChange", val, outVal, a)
                    onChange(e, value[a], a); 
                };
            }}
            onChangeCommitted={(e, val) => {
                console.log("OnChangeCommit", val)
                const value = val as number[];
                if (onChangeCommitted) { onChangeCommitted(e, value[0] !== baseValue ? value[0] : value[1]); };
            }}
        />
    );
}
*/