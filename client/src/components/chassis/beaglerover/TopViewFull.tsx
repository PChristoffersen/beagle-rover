import { range } from 'lodash';
import { LEDOutput } from '../../../services/model';
import Body from "./pieces/Body";
import Connector from "./pieces/Connector";
import LEDStrip from './pieces/LEDStrip';
import Motor from "./pieces/Motor";
import ServoArm from "./pieces/ServoArm";
import Wheel from "./pieces/Wheel";
import WheelArm from "./pieces/WheelArm";

const wheelPosX = 48.2;
const wheelPosY = 48.2;
const wheelDist = 178.2;

const servoDist = 43.84;

const wheelPositions = [
    [ wheelPosX,           wheelPosY ],
    [ wheelPosX+wheelDist, wheelPosY ],
    [ wheelPosX,           wheelPosY+wheelDist ],
    [ wheelPosX+wheelDist, wheelPosY+wheelDist ],
]

const servoPositions = [
    [ wheelPosX+servoDist,           wheelPosY+servoDist ],
    [ wheelPosX+wheelDist-servoDist, wheelPosY+servoDist ],
    [ wheelPosX+servoDist,           wheelPosY+wheelDist-servoDist ],
    [ wheelPosX+wheelDist-servoDist, wheelPosY+wheelDist-servoDist ],
]



function rotate(idx: number, angle: number) {
    switch (idx) {
        case 0:
            return "rotate("+(315.0-angle)+")"
        case 1:
            return "rotate("+(45.0-angle)+")";
        case 2:
            return "rotate("+(225.0-angle)+")";
        case 3:
            return "rotate("+(135.0-angle)+")";
    }
    return "";
}




interface Props extends React.SVGProps<SVGSVGElement> {
    wheelAngles: Array<number>;
    leds?: LEDOutput;
}


export default function TopViewFull({ wheelAngles, leds, ...props }: Props) {

    return (
        <svg
            width="100%"
            viewBox="0 0 275 275"
            xmlns="http://www.w3.org/2000/svg"
            {...props}
        >

            {range(4).map((idx) => (
                <g key={"motor"+idx} transform={"translate("+wheelPositions[idx][0]+","+wheelPositions[idx][1]+") " + rotate(idx, wheelAngles[idx])}>
                    <Motor />
                </g>
            ))}

            <Body />

            {range(4).map((idx) => (
                <g key={"wheel"+idx}>
                    <g transform={"translate("+servoPositions[idx][0]+","+servoPositions[idx][1]+") " + rotate(idx, wheelAngles[idx])}>
                        <ServoArm />
                    </g>
                    <g transform={"translate("+wheelPositions[idx][0]+","+wheelPositions[idx][1]+") " + rotate(idx, wheelAngles[idx])}>
                        <WheelArm />
                        <g transform={"translate(-18, 10) rotate("+wheelAngles[idx]+")"}>
                            <Connector />
                        </g>
                        <g transform={"translate(18, 10) rotate("+wheelAngles[idx]+")"}>
                            <Connector />
                        </g>
                    </g>


                    <g transform={"translate("+wheelPositions[idx][0]+","+wheelPositions[idx][1]+") " + rotate(idx, wheelAngles[idx])}>
                        <Wheel />
                    </g>
                </g>
            ))}    

            { leds?.front && 
                <g transform="translate(137.5, 64.5)">
                    <LEDStrip segment={leds.front} />
                </g>
            }
            { leds?.back && 
                <g transform="translate(137.5, 64.5) rotate(180, 0, 73)">
                    <LEDStrip segment={leds.back} />
                </g>
            }

        </svg>
    )
}
