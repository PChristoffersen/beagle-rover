import TopView from "./beaglerover/TopView";


interface Props {
    angle?: number;
}

export default function RobotTop({ angle=45 }: Props) {

    return (
        <TopView angle={angle}/>
    );
}