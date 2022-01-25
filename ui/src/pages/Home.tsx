import * as React from 'react';
import Box from '@mui/material/Box';
import Card from '@mui/material/Card';
import CardActions from '@mui/material/CardActions';
import CardContent from '@mui/material/CardContent';
import Typography from '@mui/material/Typography';
import { Slider, Switch, Table, TableBody, TableCell, TableContainer, TableHead, TableRow } from '@mui/material';

import Page from '../components/Page';

import { useGetMotorQuery, useSetMotorMutation } from '../services/motors'



export default function Home() {
    const index = 0
    const { data: motor, error, isLoading } = useGetMotorQuery(index)
    const [ updateMotor ] = useSetMotorMutation();


    const motorEnableChanged = (event : React.ChangeEvent<HTMLInputElement>) => {
        updateMotor({ id: index, enabled: event.target.checked });
    }
    const motorDutyChanged = (event: any, value: number | number[]) => {
        const v = value as number;
        if (motor && motor.duty !== v) {
            updateMotor({ id: index, duty: v });
        }
    }

    console.log(motor?.duty || 0.0)

    return (
        <Page>
            <Typography variant="h4" component="h4" gutterBottom>
                Hello 
            </Typography>
            <Box
                sx={{
                    display: 'flex',
                    flexWrap: 'wrap',
                    '& > :not(style)': {
                    m: 1,
                    width: 128,
                    },
                }}
            >
                <Card sx={{ minWidth: 275 }}>
                    <CardContent>
                        { isLoading && 
                            <Typography>
                                Loading
                            </Typography>
                        }
                        { !isLoading &&
                            <TableContainer>
                                <Table size="small" >
                                    <TableHead>
                                        <TableRow>
                                            <TableCell>Name</TableCell>
                                            <TableCell align='right'>Value</TableCell>
                                        </TableRow>
                                    </TableHead>
                                    <TableBody>
                                        <TableRow>
                                            <TableCell>Enabled</TableCell>
                                            <TableCell align='right'>{motor?.enabled?"Yes":"No"}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell>Duty</TableCell>
                                            <TableCell align='right'>{motor && motor.duty.toPrecision(2)}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell>RPM</TableCell>
                                            <TableCell align='right'>{motor && motor.rpm.toPrecision(2)}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell>Odometer</TableCell>
                                            <TableCell align='right'>{motor?.odometer}</TableCell>
                                        </TableRow>
                                    </TableBody>
                                </Table>
                            </TableContainer>
                        }
                    </CardContent>
                    <CardActions>
                        <Switch checked={motor?.enabled} onChange={motorEnableChanged} />
                        <Slider 
                            value={motor?.duty || 0.0} 
                            disabled={!(!!motor && motor.enabled)} 
                            step={0.1}
                            min={-1.0} 
                            max={1.0} 
                            marks
                            onChangeCommitted={motorDutyChanged} 
                        />
                    </CardActions>
                </Card>
            </Box>
        </Page>
    )
}