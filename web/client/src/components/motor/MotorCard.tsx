import { Card, CardActions, CardHeader, CardContent, Box, Switch, Table, TableBody, TableCell, TableContainer, TableHead, TableRow, CircularProgress } from '@mui/material';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

import { useGetMotorQuery, useSetMotorMutation } from "../../services/motors";
import MotorDutySlider from './MotorDutySlider';



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

function dutyTextTable(value: number|undefined): string {
    if (value === undefined) {
        return ""
    }
    return float2string(100*value, 0)
}


export function MotorCardBody({ id, disabled }: Props) {
    const { data: motor, isError, isLoading, isSuccess } = useGetMotorQuery(id)
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

    return (
        <>
            <CardHeader title={"Motor "+id} action={ isSuccess && !disabled &&
                <Switch checked={motor?.enabled || false} onChange={enableChanged} />
            }/>
            <CardContent>
                { !isSuccess && 
                    <Box display="flex" justifyContent="center" alignItems="center">
                        { isError && <ErrorOutlineIcon color="error" fontSize="large" /> }
                        { !isError && isLoading && <CircularProgress /> }
                    </Box>
                }
                { isSuccess &&
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
                                    <TableCell sx={{ paddingRight: 0.5 }} align='right'>{motor?.odometer}</TableCell>
                                    <TableCell padding='none'  align='left' width='fit-content'>mm</TableCell>
                                </TableRow>
                                <TableRow>
                                    <TableCell>Encoder</TableCell>
                                    <TableCell sx={{ paddingRight: 0.5 }} align='right'>{motor?.encoder}</TableCell>
                                    <TableCell padding='none' align='left' width='fit-content'>tck</TableCell>
                                </TableRow>
                            </TableBody>
                        </Table>
                    </TableContainer>
                }
            </CardContent>
            <CardActions>
            { isSuccess && !disabled && 
                <Box sx={{ width: '100%' }}>
                    <MotorDutySlider id={id} disabled={disabled} />
                </Box>
            }
            </CardActions>
        </>
    )
}


export default function MotorCard({ id, disabled }: Props) {
    return (
        <Card sx={{ minWidth: 275 }}>
            <MotorCardBody id={id} disabled={disabled} />
        </Card>
    )

}