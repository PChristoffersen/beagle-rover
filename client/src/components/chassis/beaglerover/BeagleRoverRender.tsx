import { Box, BoxProps } from "@mui/material";
import { RobotRenderProps, RobotRenderViewType } from "../RobotRender";


function view2src(view: RobotRenderViewType): string {
    switch (view) {
        case "top":
            return "/images/beaglerover/top.png";
        case "default":
        default:
            return "/images/beaglerover/default.png";
    }
}


export default function BeagleRoverRender({ view="default", ...props }: RobotRenderProps) {
    const src = view2src(view);

    return (
        <Box 
            component="img"
            src={src}
            {...props}
        />
    )
}