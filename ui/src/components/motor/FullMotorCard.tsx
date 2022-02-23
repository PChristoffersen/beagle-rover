import { Card, Divider } from "@mui/material";
import { MotorCardBody } from "./MotorCard";
import { ServoCardBody } from "./ServoCard";

interface Props {
    id: number
    disabled?: boolean
}

export default function FullMotorCard({ id, disabled }: Props) {
    return (
        <Card sx={{ minWidth: 275 }}>
            <MotorCardBody id={id} disabled={disabled} />
            <ServoCardBody id={id} disabled={disabled} />
       </Card>
    )
}