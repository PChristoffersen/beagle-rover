import { Grid, useTheme } from '@mui/material';
import { useEffect, useRef } from 'react';
import { Joystick, JoystickShape } from 'react-joystick-component';
import { Socket } from 'socket.io-client';
import { createSocket, steer } from '../../services/input';

export default function JoystickSteering() {
    const { palette } = useTheme();
    const sockRef = useRef<Socket|null>(null);

    useEffect(() => {
        console.log("Joystick Enter");
        sockRef.current = createSocket();

        return () => {
            console.log("Joystick Exit");
            sockRef.current?.disconnect()
            sockRef.current = null;
        }
    }, []);


    const handleMove = (x: number, y: number) => {
        x = x/50.0;
        y = y/50.0;
        const sock = sockRef.current;
        if (sock) {
            steer(sock, { direction: x, throttle: y });
        }

    };

    return (
        <Grid container alignItems="center" justifyContent="center" sx={{ minWidth: 200, minHeight: 200}}>
            <Grid item>
                <Joystick 
                    size={150}
                    baseColor={palette.background.paper}
                    stickColor={palette.primary.main}
                    throttle={20}
                    minDistance={0}
                    move={(event) => handleMove(event.x || 0.0, event.y || 0.0)}
                    stop={(event) => handleMove(0.0, 0.0)}
                    controlPlaneShape={JoystickShape.Square}
                />
            </Grid>
        </Grid>
    );
}