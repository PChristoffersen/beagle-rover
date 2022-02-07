import { Box, Tooltip, Typography } from "@mui/material";
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
    const { data: receiver, isSuccess } = useGetRCReceiverQuery();

    const rssi = receiver?.rssi || 0;

    if (!isSuccess) {
        return null;
    }

    var tooltip = "rssi = "+rssi;
    var icon = null;
    var text = rssi + "%";
    
    if (!receiver?.enabled) {
        tooltip = "Disabled";
        text = "";
        icon = <SignalCellularOffIcon fontSize='large' />;
    }
    else if (!receiver?.connected) {
        tooltip = "No signal";
        text = "No signal";
        icon = <SignalCellularNodataIcon fontSize='large' />;
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
        icon = <SignalCellular1BarIcon fontSize='large' />;
    }
    else {
        icon = <SignalCellular0BarIcon fontSize='large' />;
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
                { text && <Box sx={{ display: { xs: 'none', md: 'flex' }}}>
                    <Typography>
                        {text}
                    </Typography>
                </Box>}
            </Box>
        </Tooltip>
    )
}
