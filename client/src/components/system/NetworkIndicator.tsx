import { Box, Tooltip, Typography } from "@mui/material";
import SignalWifi0BarIcon from '@mui/icons-material/SignalWifi0Bar';
import SignalWifi1BarIcon from '@mui/icons-material/SignalWifi1Bar';
import SignalWifi2BarIcon from '@mui/icons-material/SignalWifi2Bar';
import SignalWifi3BarIcon from '@mui/icons-material/SignalWifi3Bar';
import SignalWifi4BarIcon from '@mui/icons-material/SignalWifi4Bar';
import SignalWifiBadIcon from '@mui/icons-material/SignalWifiBad';
import SignalWifiOffIcon from '@mui/icons-material/SignalWifiOff';
import CableIcon from '@mui/icons-material/Cable';
import WifiTetheringIcon from '@mui/icons-material/WifiTethering';
import UsbIcon from '@mui/icons-material/Usb';

import { useGetSystemQuery } from "../../services/system";


export default function NetworkIndicator() {
    const { data: system, isSuccess } = useGetSystemQuery()

    if (!isSuccess || !system?.network) {
        return null;
    }

    const network = system.network

    var icon = null;
    var tooltip = "";
    var text = null;

    //icon = <SignalWifiBadIcon fontSize='large' />;
    icon = <WifiTetheringIcon fontSize='large' />;

    return (
        <Tooltip arrow title={tooltip}>
            <Box sx={{ 
                display: 'flex', 
                height: 'fit-content', 
                width: 'fit-content', 
                alignItems: 'flex-end',
            }}>
                {icon}
                <Box sx={{ display: { xs: 'none', md: 'flex' }}}>
                    <Typography>
                        {text}
                    </Typography>
                </Box>
            </Box>
        </Tooltip>
    )
}
