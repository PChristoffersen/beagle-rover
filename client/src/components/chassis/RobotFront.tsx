import FrontView from "./beaglerover/FrontView";


interface Props {
    angle?: number;
}

export default function RobotFront({ angle=45 }: Props) {
    return (
        <FrontView angle={angle} />
    );
}