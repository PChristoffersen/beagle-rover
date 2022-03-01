import { Box, Tooltip, Typography, useTheme } from "@mui/material";
import SignalCellularNodataIcon from '@mui/icons-material/SignalCellularNodata';
import SignalCellularOffIcon from '@mui/icons-material/SignalCellularOff';
import SignalCellular0BarIcon from '@mui/icons-material/SignalCellular0Bar';
import SignalCellular1BarIcon from '@mui/icons-material/SignalCellular1Bar';
import SignalCellular2BarIcon from '@mui/icons-material/SignalCellular2Bar';
import SignalCellular3BarIcon from '@mui/icons-material/SignalCellular3Bar';
import SignalCellular4BarIcon from '@mui/icons-material/SignalCellular4Bar';

import { useGetRCReceiverQuery } from "../../services/rcreceiver";
import React from "react";


export default function SignalIndicator() {
    const { palette } = useTheme();
    const { data: receiver, isSuccess } = useGetRCReceiverQuery();

    const rssi = receiver?.rssi || 0;
    const enabled = receiver?.enabled || false;
    const connected = receiver?.connected || false;

    if (!isSuccess) {
        return null;
    }

    var tooltip = "rssi = "+rssi;
    var icon = null;
    var text = <Typography>{rssi + "%"}</Typography>;

    if (!enabled) {
        tooltip = "Disabled";
        text = <></>;
        icon = <SignalCellularOffIcon fontSize='large' />;
    }
    else if (!connected) {
        tooltip = "No signal";
        text = <Typography color={palette.warning.main}>No signal</Typography>;
        icon = <SignalCellularNodataIcon fontSize='large' color="warning" />;
    }
    else if (rssi >= 80) {
        icon = <SignalCellular4BarIcon fontSize='large' />;
    }
    else if (rssi >= 60) {
        icon = <SignalCellular3BarIcon fontSize='large' />;
    }
    else if (rssi >= 40) {
        icon = <SignalCellular2BarIcon fontSize='large' />;
    }
    else if (rssi >= 20) {
        text = <Typography color={palette.warning.main}>{rssi + "%"}</Typography>;
        icon = <SignalCellular1BarIcon fontSize='large' color="warning" />;
    }
    else {
        text = <Typography color={palette.error.main}>{rssi + "%"}</Typography>;
        icon = <SignalCellular0BarIcon fontSize='large' color="error" />;
    }

    return (
        <Tooltip arrow title={tooltip}>
            <Box sx={{ 
                display: 'flex', 
                height: 'fit-content', 
                width: 'fit-content',
                alignItems: 'flex-end',
            }}>
                {icon}
                { text && 
                    <Box sx={{ display: { xs: 'none', md: 'flex' }}}>
                        {text}
                    </Box>
                }
            </Box>
        </Tooltip>
    )
}
