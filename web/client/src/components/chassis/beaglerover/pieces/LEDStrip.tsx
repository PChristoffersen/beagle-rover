import { Color, ColorSegment } from "../../../../services/model";
import { DefaultColors } from "../Colors";


interface Props {
    segment: ColorSegment
}

export default function LEDStrip({ segment }: Props) {
    const ledSize = 3;
    const ledSpacing = 2*ledSize+1;

    const ledX = -((segment.length-1)*ledSpacing+ledSize*2)/2;
    const ledY = -ledSize;

    const strokeColor = DefaultColors.strokeColor;

    return (
        <g transform={"translate("+ledX+","+ledY+")"}>
            {segment.map((color: Color, index: number) => (
            <circle key={"led_segment"+index}
                style={{
                    fill: "#"+color,
                    stroke: strokeColor,
                    strokeWidth: 0.5,
                }}
                cx={ledSize+index*ledSpacing}
                cy={ledSize}
                r={ledSize}
            />
            ))}
        </g>
    );
}