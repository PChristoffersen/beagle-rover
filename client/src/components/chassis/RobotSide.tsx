import SideView from "./beaglerover/SideView";


interface Props {
    angle?: number;
}

export default function RobotSide({ angle=45 }: Props) {

    return (
        <SideView angle={angle}/>
    );
}