import { BoxProps } from "@mui/material";
import BeagleRoverRender from "./beaglerover/BeagleRoverRender";


export type RobotRenderViewType = "default" | "top" | "front" | "side";

export interface RobotRenderProps extends BoxProps {
    view?: RobotRenderViewType;
}


export default function RobotRender(props: RobotRenderProps) {
    return (
        <BeagleRoverRender {...props} />
    );
}