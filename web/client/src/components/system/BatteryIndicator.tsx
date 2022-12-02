import { Box, Tooltip, Typography, useTheme } from "@mui/material";
import Battery20Icon from '@mui/icons-material/Battery20';
import Battery30Icon from '@mui/icons-material/Battery30';
import Battery50Icon from '@mui/icons-material/Battery50';
import Battery60Icon from '@mui/icons-material/Battery60';
import Battery80Icon from '@mui/icons-material/Battery80';
import Battery90Icon from '@mui/icons-material/Battery90';
import BatteryFullIcon from '@mui/icons-material/BatteryFull';
import BatteryCharging20Icon from '@mui/icons-material/BatteryCharging20';
import BatteryCharging30Icon from '@mui/icons-material/BatteryCharging30';
import BatteryCharging50Icon from '@mui/icons-material/BatteryCharging50';
import BatteryCharging60Icon from '@mui/icons-material/BatteryCharging60';
import BatteryCharging80Icon from '@mui/icons-material/BatteryCharging80';
import BatteryCharging90Icon from '@mui/icons-material/BatteryCharging90';
import BatteryChargingFullIcon from '@mui/icons-material/BatteryChargingFull';
import PowerIcon from '@mui/icons-material/Power';

import { useGetPowerQuery } from "../../services/system";
import { PowerSourceBattery, PowerSourceType } from "../../services/model";


export default function BatteryIndicator() {
    const { palette } = useTheme();
    const { data: power, isSuccess } = useGetPowerQuery()

    if (!isSuccess) {
        return null;
    }

    const system_power = power.system

    var icon = null;
    var tooltip = "";
    var text = <></>;

    if (system_power?.type===PowerSourceType.UNKNOWN) {
        //const source = system_power as PowerSourceUnknown;
        tooltip = "Unknown";
        icon = <PowerIcon fontSize='large' />;
    }
    else if (system_power?.type === PowerSourceType.BATTERY) {
        const battery = system_power as PowerSourceBattery;
        const charge = (battery.percent*100.0).toLocaleString(undefined, { minimumFractionDigits: 0, maximumFractionDigits: 0 })
        const voltage = battery.voltage.toLocaleString(undefined, { minimumFractionDigits: 2, maximumFractionDigits: 2 })

        if (battery.charging) {
            tooltip = "Charging " + charge + "%   ( " + voltage + "v )";
            text = <Typography>Charging</Typography>;
            if (battery.percent <= 0.2) {
                icon = <BatteryCharging20Icon fontSize='large'/>
            }
            else if (battery.percent <= 0.3) {
                icon = <BatteryCharging30Icon fontSize='large' />
            }
            else if (battery.percent <= 0.5) {
                icon = <BatteryCharging50Icon fontSize='large' />
            }
            else if (battery.percent <= 0.6) {
                icon = <BatteryCharging60Icon fontSize='large' />
            }
            else if (battery.percent <= 0.8) {
                icon = <BatteryCharging80Icon fontSize='large' />
            }
            else if (battery.percent <= 0.9) {
                icon = <BatteryCharging90Icon fontSize='large' />
            }
            else {
                text = <Typography>Charged</Typography>;
                icon = <BatteryChargingFullIcon fontSize='large' />
            }
        }
        else {
            const charge_text = charge+"%";
            tooltip = "" + charge + "%   ( " + voltage + "v )";
            text = <Typography>{charge_text}</Typography>;

            if (battery.percent <= 0.2) {
                text = <Typography color={palette.error.main}>{charge_text}</Typography>;
                icon = <Battery20Icon fontSize='large' color="error" />
            }
            else if (battery.percent <= 0.3) {
                text = <Typography color={palette.warning.main}>{charge_text}</Typography>;
                icon = <Battery30Icon fontSize='large' color="warning"/>
            }
            else if (battery.percent <= 0.5) {
                icon = <Battery50Icon fontSize='large' />
            }
            else if (battery.percent <= 0.6) {
                icon = <Battery60Icon fontSize='large' />
            }
            else if (battery.percent <= 0.8) {
                icon = <Battery80Icon fontSize='large' />
            }
            else if (battery.percent <= 0.9) {
                icon = <Battery90Icon fontSize='large' />
            }
            else {
                icon = <BatteryFullIcon fontSize='large' />
            }
        }
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
