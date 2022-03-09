import { useEffect } from 'react';
import { LEDOutput } from "../../services/model";
import { useGetMotorQuery } from "../../services/motors";
import TopView from "./beaglerover/TopView";


interface Props {
    leds?: LEDOutput;
}

export default function RobotTop({ leds }: Props) {
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
        <TopView wheelAngles={wheelAngles} leds={leds}/>
    );
}