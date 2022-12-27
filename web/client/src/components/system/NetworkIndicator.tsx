import { Tooltip, Typography } from "@mui/material";
import SignalWifi0BarIcon from '@mui/icons-material/SignalWifi0Bar';
import SignalWifi1BarIcon from '@mui/icons-material/SignalWifi1Bar';
import SignalWifi2BarIcon from '@mui/icons-material/SignalWifi2Bar';
import SignalWifi3BarIcon from '@mui/icons-material/SignalWifi3Bar';
import SignalWifi4BarIcon from '@mui/icons-material/SignalWifi4Bar';
import WifiTetheringIcon from '@mui/icons-material/WifiTethering';
import UsbIcon from '@mui/icons-material/Usb';
import SettingsEthernetIcon from '@mui/icons-material/SettingsEthernet';
import { useGetNetworkQuery } from "../../services/system";
import { NetworkInterfaceBase, NetworkInterfaceEthernet, NetworkInterfaceType, NetworkInterfaceUSB, NetworkInterfaceWifi, NetworkInterfaceWifiAP } from "../../services/model";



interface NetworkInterfaceIconProps {
    iface: NetworkInterfaceEthernet|NetworkInterfaceWifi|NetworkInterfaceWifiAP|NetworkInterfaceUSB;
    fontSize: 'inherit' | 'large' | 'medium' | 'small';
}


function NetworkInterfaceIcon({ iface, fontSize }: NetworkInterfaceIconProps) {
    //console.log("Network", iface)


    if (iface.type===NetworkInterfaceType.ETHERNET) {
        const iface_ = iface as NetworkInterfaceEthernet;

        return (
            <Tooltip 
                arrow 
                title={
                    <> 
                        <Typography variant="h6">
                            Ethernet {iface_.name}
                        </Typography>
                        <Typography variant="subtitle2">
                            Mac:
                        </Typography>
                        <Typography variant="caption">
                            {iface_.mac}
                        </Typography>
                        <Typography variant="subtitle2">
                            Address: 
                        </Typography>
                        { iface_.addresses.map(addr => (
                            <Typography variant="caption">
                                {addr}
                            </Typography>
                        ))}
                    </>
                }
            >
                <SettingsEthernetIcon fontSize={fontSize}/>
            </Tooltip>
        )
    
    }

    if (iface.type===NetworkInterfaceType.WIRELESS) {
        const iface_ = iface as NetworkInterfaceWifi;

        var icon = null;

        if (iface_.signal_rssi < 10) {
            icon = <SignalWifi0BarIcon fontSize={fontSize}/>;
        }
        else if (iface_.signal_rssi < 30) {
            icon = <SignalWifi1BarIcon fontSize={fontSize}/>;
        }
        else if (iface_.signal_rssi < 60) {
            icon = <SignalWifi2BarIcon fontSize={fontSize}/>;
        }
        else if (iface_.signal_rssi < 90) {
            icon = <SignalWifi3BarIcon fontSize={fontSize}/>;
        }
        else {
            icon = <SignalWifi4BarIcon fontSize={fontSize}/>;
        }

        return (
            <Tooltip 
                arrow 
                title={
                    <> 
                        <Typography variant="h6">
                            Wireless {iface_.name}
                        </Typography>
                        <Typography variant="subtitle2">
                            SSID:
                        </Typography>
                        <Typography variant="caption">
                            {iface_.ssid}
                        </Typography>
                        <Typography variant="subtitle2">
                            Signal:
                        </Typography>
                        <Typography variant="caption">
                            {iface_.signal_dbm} dBm
                        </Typography>
                        { iface_.frequency && <>
                            <Typography variant="subtitle2">
                                Channel:
                            </Typography>
                            <Typography variant="caption">
                                channel {iface_.channel} ({iface_.frequency} MHz), width: {iface_.channel_width}
                            </Typography>
                        </>}
                        <Typography variant="subtitle2">
                            Address: 
                        </Typography>
                        { iface_.addresses.map(addr => (
                            <Typography variant="caption">
                                {addr}
                            </Typography>
                        ))}
                    </>
                }
            >
                {icon}
            </Tooltip>
        )
    
    }

    if (iface.type===NetworkInterfaceType.WIRELESS_AP) {
        const iface_ = iface as NetworkInterfaceWifiAP;

        return (
            <Tooltip 
            arrow 
            title={
                <> 
                    <Typography variant="h6">
                        Wireless AP {iface_.name}
                    </Typography>
                    <Typography variant="subtitle2">
                        SSID:
                    </Typography>
                    <Typography variant="caption">
                        {iface_.ssid}
                    </Typography>
                    { iface_.frequency && <>
                        <Typography variant="subtitle2">
                            Channel:
                        </Typography>
                        <Typography variant="caption">
                            channel {iface_.channel} ({iface_.frequency} MHz), width: {iface_.channel_width}
                        </Typography>
                    </>}
                    <Typography variant="subtitle2">
                        Address: 
                    </Typography>
                    { iface_.addresses.map(addr => (
                        <Typography variant="caption">
                            {addr}
                        </Typography>
                    ))}
                </>
            }
        >
            <WifiTetheringIcon fontSize={fontSize} />
        </Tooltip>
    )
}

    if (iface.type===NetworkInterfaceType.USB_GADGET) {
        const iface_ = iface as NetworkInterfaceUSB;
        return (
            <Tooltip 
                arrow 
                title={
                    <> 
                        <Typography variant="h6">
                            Ethernet {iface_.name}
                        </Typography>
                        <Typography variant="subtitle2">
                            Mac:
                        </Typography>
                        <Typography variant="caption">
                            {iface_.mac}
                        </Typography>
                        <Typography variant="subtitle2">
                            Address: 
                        </Typography>
                        { iface_.addresses.map((addr: string) => (
                            <Typography variant="caption">
                                {addr}
                            </Typography>
                        ))}
                    </>
                }
            >
                <UsbIcon fontSize={fontSize}/>
            </Tooltip>
        )
    }

    return null;
}


export default function NetworkIndicator() {
    const { data: network, isSuccess } = useGetNetworkQuery()

    if (!isSuccess || !network) {
        return null;
    }

    return (
        <>
            { Object.entries(network).map(([name, iface]) => (
                (iface as NetworkInterfaceBase).active && <NetworkInterfaceIcon key={name} iface={iface} fontSize="large" />
            ))}
        </>
    )
}
