import { Card, CardActions, CardHeader, CardContent, Box, Switch, Table, TableBody, TableCell, TableContainer, TableHead, TableRow, CircularProgress } from '@mui/material';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

import { useGetMotorQuery, useSetMotorMutation } from "../../services/motors";
import ServoAngleSlider from './ServoAngleSlider';



interface Props {
    id: number, 
    disabled?: boolean
}

function float2string(val: number|undefined, digits: number): string {
    if (val === undefined) {
        return ""
    }
    return val.toLocaleString(undefined, { minimumFractionDigits: digits, maximumFractionDigits: digits })
}

function angleTextTable(value: number|undefined): string {
    if (value === undefined) {
        return "0.0"
    }
    return float2string(value * 180.0 / Math.PI, 1)
}



export function ServoCardBody({ id, disabled }: Props) {
    const { data: motor, error, isLoading } = useGetMotorQuery(id)
    const [ update ] = useSetMotorMutation();
    const servo = motor?.servo;

    const enableChanged = (event : React.ChangeEvent<HTMLInputElement>) => {
        const v = event.target.checked;
        if (v) {
            update({ id: id, servo: { enabled: v } });
        }
        else {
            update({ id: id, servo: { enabled: v, angle: 0.0 } });
        }
    }

    const isReady = !error && !isLoading
    const servoAngle = servo?.angle || 0.0;

    return (
        <>
            <CardHeader title={"Servo "+id} action={ isReady && !disabled &&
                <Switch checked={servo?.enabled || false} onChange={enableChanged} />
            }/>
            <CardContent>
                { !isReady && 
                    <Box display="flex" justifyContent="center" alignItems="center">
                        { error && <ErrorOutlineIcon color="error" fontSize="large" /> }
                        { !error && isLoading && <CircularProgress /> }
                    </Box>
                }
                { isReady &&
                    <TableContainer>
                        <Table size="small" >
                            <TableHead>
                                <TableRow>
                                    <TableCell>Name</TableCell>
                                    <TableCell colSpan={2} align='right'>Value</TableCell>
                                </TableRow>
                            </TableHead>
                            <TableBody>
                                <TableRow>
                                    <TableCell>Angle</TableCell>
                                    <TableCell sx={{ paddingRight: 0.5 }} align='right'>{angleTextTable(servoAngle)}</TableCell>
                                    <TableCell padding='none'  align='left' width='fit-content'>°</TableCell>
                                </TableRow>
                                <TableRow>
                                    <TableCell>Pulse width</TableCell>
                                    <TableCell sx={{ paddingRight: 0.5 }} align='right'>{float2string(servo?.pulse_us, 0)}</TableCell>
                                    <TableCell padding='none'  align='left' width='fit-content'>us</TableCell>
                                </TableRow>
                            </TableBody>
                        </Table>
                    </TableContainer>
                }
            </CardContent>
            <CardActions>
            { isReady && !disabled && 
                <Box sx={{ width: '100%', paddingLeft: 2, paddingRight: 2 }}>
                    <ServoAngleSlider id={id} disabled={disabled} />
                </Box>
            }
            </CardActions>
        </>
    )
}


export default function ServoCard({ id, disabled }: Props) {
    return (
        <Card sx={{ minWidth: 275 }}>
            <ServoCardBody id={id} disabled={disabled} />
       </Card>
    );
}