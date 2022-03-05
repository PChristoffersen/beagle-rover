import { range } from 'lodash';
import Body from "./pieces/Body";
import Connector from "./pieces/Connector";
import Motor from "./pieces/Motor";
import ServoArm from "./pieces/ServoArm";
import Wheel from "./pieces/Wheel";
import WheelArm from "./pieces/WheelArm";
import { useGetMotorQuery } from '../../../services/motors';


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




export default function TopView() {
    const { data: motor0 } = useGetMotorQuery(0)
    const { data: motor1 } = useGetMotorQuery(1)
    const { data: motor2 } = useGetMotorQuery(2)
    const { data: motor3 } = useGetMotorQuery(3)
    
    const wheelAngles = [
        motor0?.servo?.angle || 0.0,
        motor1?.servo?.angle || 0.0,
        motor2?.servo?.angle || 0.0,
        motor3?.servo?.angle || 0.0,
    ]



    return (
        <svg
            width="100%"
            viewBox="0 0 275 275"
            xmlns="http://www.w3.org/2000/svg"
        >

            {range(4).map((idx) => (
                <g id={"motor"+idx} transform={"translate("+wheelPositions[idx][0]+","+wheelPositions[idx][1]+") " + rotate(idx, wheelAngles[idx])}>
                    <Motor />
                </g>
            ))}

            <Body />

            {range(4).map((idx) => (
                <g id={"wheels"+idx}>
                    <g transform={"translate("+servoPositions[idx][0]+","+servoPositions[idx][1]+") " + rotate(idx, wheelAngles[idx])}>
                        <ServoArm />
                    </g>
                    <g id={"wheel"+idx} transform={"translate("+wheelPositions[idx][0]+","+wheelPositions[idx][1]+") " + rotate(idx, wheelAngles[idx])}>
                        <WheelArm />
                        <g transform={"translate(-18, 10) rotate("+wheelAngles[idx]+")"}>
                            <Connector />
                        </g>
                        <g transform={"translate(18, 10) rotate("+wheelAngles[idx]+")"}>
                            <Connector />
                        </g>
                    </g>


                    <g id={"wheel"+idx} transform={"translate("+wheelPositions[idx][0]+","+wheelPositions[idx][1]+") " + rotate(idx, wheelAngles[idx])}>
                        <Wheel />
                    </g>
                </g>
            ))}    

        </svg>
    )
}
