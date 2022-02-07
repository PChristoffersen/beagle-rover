import { Box, Tooltip } from "@mui/material";
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
    const { data: receiver, error, isLoading } = useGetRCReceiverQuery();

    const isReady = !error && !isLoading;
    const rssi = receiver?.rssi || 0;

    if (!isReady) {
        return null;
    }

    var tooltip = "rssi = "+rssi;
    var icon = null;
    if (!receiver?.enabled) {
        tooltip = "Disabled";
        icon = <SignalCellularOffIcon fontSize='large' />;
    }
    else if (!receiver?.connected) {
        tooltip = "Not connected";
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
            <Box sx={{ display: 'flex', height: 'fit-content', width: 'fit-content' }}>
                {icon}
            </Box>
        </Tooltip>
    )
}
