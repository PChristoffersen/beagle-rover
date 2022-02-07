import { Card, CardActions, CardHeader, CardContent, Box, Slider, Switch, Table, TableBody, TableCell, TableContainer, TableHead, TableRow, CircularProgress } from '@mui/material';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

import { useGetMotorQuery, useSetMotorMutation } from "../../services/motors";



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

function angleText(value: number|undefined): string {
    if (value === undefined) {
        return ""
    }
    return float2string(value * 180.0 / Math.PI, 1) + "°"
}

function angleTextTable(value: number|undefined): string {
    if (value === undefined) {
        return "0.0"
    }
    return float2string(value * 180.0 / Math.PI, 1)
}





export default function ServoCard({ id, disabled }: Props) {
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
    const angleChanged = (event: any, value: number | number[]) => {
        const v = (value as number);
        if (servo && servo.angle !== v) {
            update({ id: id, servo: { angle: v } });
        }
    }

    const isReady = !error && !isLoading
    const servoAngle = servo?.angle || 0.0;
    const servoEnabled = servo?.enabled || false;
    const limitMin = servo?.limit_min || -Math.PI/2.0;
    const limitMax = servo?.limit_max || Math.PI/2.0;
    const step = (limitMax-limitMin)/180.0;

    const marks = [
        {
            value: limitMin,
            label: angleText(limitMin)
        },
        {
            value: 0.0,
            label: angleText(0.0)
        },
        {
            value: limitMax,
            label: angleText(limitMax)
        },
    ]
    
    return (
        <Card sx={{ minWidth: 275 }}>
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
                    <Slider 
                        value={servoAngle} 
                        disabled={!servoEnabled}
                        step={step}
                        min={limitMin} 
                        max={limitMax} 
                        valueLabelFormat={angleText}
                        valueLabelDisplay='auto'
                        marks={marks}
                        onChangeCommitted={angleChanged} 
                    />
                </Box>
            }
            </CardActions>
        </Card>
    );
}