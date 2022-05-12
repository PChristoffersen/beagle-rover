import { Box, Card, CardContent, CardHeader, Grid, Typography, useTheme } from "@mui/material";
import TopViewFull from "../components/chassis/beaglerover/TopViewFull";
import TopView from "../components/chassis/beaglerover/TopView";
import Page from "../components/Page";
import { useGetOutputQuery } from "../services/leds";
import { useGetHistoryQuery } from "../services/telemetry";
import { useState } from "react";
import { TelemetryHistory, TelemetryHistorySet } from "../services/model";
import { Line, LineChart, ReferenceLine, XAxis, YAxis } from "recharts";


function radian2degrees(a: number): number {
    return (180.0*a)/Math.PI;
}

function convert_imu(data: TelemetryHistory): any {
    let res = [];
    const length = data.buffer.length;
    const head = data.head;
    for (let i=0; i<length; i++) {
        const el = data.buffer[(head+i)%length];
        res.push({ 
            index: length-i,
            timestamp: ((i-length)*data.interval),
            pitch: radian2degrees(el[0]),
            roll: radian2degrees(el[1]),
            yaw: radian2degrees(el[2]),
        })
    }
    return res;
}

/*
function convert_combined(data: TelemetryHistorySet): any {
    let res = [];

    

    return res;
}
*/


export default function Test() {
    const { data, isSuccess } = useGetHistoryQuery([ 'motor_duty', 'imu' ], { pollingInterval: 250 })

    const dataImu = data?convert_imu(data.imu):{}

    return (
        <Page>
            <Box>
                <LineChart width={800} height={400} data={dataImu}>
                    <YAxis type="number" domain={[ -90, 90 ]} />
                    <ReferenceLine y={0.0} stroke="red" strokeDasharray="3 3" />
                    <Line type="monotone" dataKey="pitch" stroke="#8884d8" dot={false} />
                    <Line type="monotone" dataKey="roll" stroke="#8884d8" dot={false} />
                    <Line type="monotone" dataKey="yaw" stroke="#8884d8" dot={false} />
                </LineChart>

                <Box
                    sx={{
                        height: "100%",
                        width: "100%",
                        backgroundColor: "blue",
                    }}
                >
                    Flup
                </Box>
            </Box>
        </Page>
    );
}