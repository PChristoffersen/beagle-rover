import { Card, CardActions, CardHeader, CardContent, Box, Slider, Switch, Table, TableBody, TableCell, TableContainer, TableHead, TableRow, CircularProgress } from '@mui/material';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

import { useGetMotorQuery, useSetMotorMutation } from "../../services/motors";



interface Props {
    id: number
    disabled?: boolean
}

function float2string(val: number|undefined, digits: number): string {
    if (val === undefined) {
        return ""
    }
    return val.toLocaleString(undefined, { minimumFractionDigits: digits, maximumFractionDigits: digits })
}

function dutyText(value: number|undefined): string {
    if (value === undefined) {
        return ""
    }
    return float2string(100*value, 0)+"%"
}

function dutyTextTable(value: number|undefined): string {
    if (value === undefined) {
        return ""
    }
    return float2string(100*value, 0)
}



export default function MotorCard({ id, disabled }: Props) {
    const { data: motor, error, isLoading } = useGetMotorQuery(id)
    const [ update ] = useSetMotorMutation();

    const enableChanged = (event : React.ChangeEvent<HTMLInputElement>) => {
        const v = event.target.checked
        if (v) {
            update({ id: id, enabled: v });
        }
        else {
            update({ id: id, enabled: v, duty: 0.0 });
        }
    }
    const dutyChanged = (event: any, value: number | number[]) => {
        const v = value as number;
        if (motor && motor.duty !== v) {
            update({ id: id, duty: v });
        }
    }

    const isReady = !error && !isLoading
    const motorDuty = (motor?.duty || 0.0);
    const motorEnabled = motor?.enabled || false;
    const limitMin = -1.0;
    const limitMax =  1.0;
    const step = 0.01;

    const marks = [
        {
            value: limitMin,
            label: dutyText(limitMin)
        },
        {
            value: 0.0,
            label: dutyText(0.0)
        },
        {
            value: limitMax,
            label: dutyText(limitMax)
        },
    ]

    return (
        <Card sx={{ minWidth: 275 }}>
            <CardHeader title={"Motor "+id} action={ isReady && !disabled &&
                <Switch checked={motor?.enabled || false} onChange={enableChanged} />
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
                                    <TableCell>Duty</TableCell>
                                    <TableCell sx={{ paddingRight: 0.5 }} align='right'>{dutyTextTable(motor?.duty)}</TableCell>
                                    <TableCell padding='none'  align='left' width='fit-content'>% </TableCell>
                                </TableRow>
                                <TableRow>
                                    <TableCell>Speed</TableCell>
                                    <TableCell sx={{ paddingRight: 0.5 }} align='right'>{float2string(motor?.rpm, 0)}</TableCell>
                                    <TableCell padding='none'  align='left' width='fit-content'>rpm</TableCell>
                                </TableRow>
                                <TableRow>
                                    <TableCell>Odometer</TableCell>
                                    <TableCell sx={{ paddingRight: 0.5 }} align='right'>{float2string(motor?.odometer, 0)}</TableCell>
                                    <TableCell padding='none'  align='left' width='fit-content'>mm</TableCell>
                                </TableRow>
                                <TableRow>
                                    <TableCell>Encoder</TableCell>
                                    <TableCell sx={{ paddingRight: 0.5 }} align='right'>{float2string(motor?.encoder, 0)}</TableCell>
                                    <TableCell padding='none' align='left' width='fit-content'>tck</TableCell>
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
                        value={motorDuty}
                        disabled={!motorEnabled}
                        step={step}
                        min={limitMin} 
                        max={limitMax} 
                        valueLabelFormat={dutyText}
                        valueLabelDisplay='auto'
                        marks={marks}
                        onChangeCommitted={dutyChanged} 
                    />
                </Box>
            }
            </CardActions>
        </Card>
    )

}