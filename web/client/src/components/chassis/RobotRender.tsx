import { Box, BoxProps } from "@mui/material";


export type RobotRenderViewType = "default" | "top" | "front" | "side";

export interface RobotRenderProps extends BoxProps {
    view?: RobotRenderViewType;
}


function view2src(view: RobotRenderViewType): string {
    switch (view) {
        case "top":
            return "/images/beaglerover/top.png";
        case "default":
        default:
            return "/images/beaglerover/default.png";
    }
}



export default function RobotRender({ view="default", ...props }: RobotRenderProps) {
    const src = view2src(view);

    return (
        <Box
            component="img"
            src={src}
            {...props}
        />
    )
}