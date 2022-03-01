import { Stack } from '@mui/material';
import SignalIndicator from '../../components/rcremote/SignalIndicator';
import BatteryIndicator from '../../components/system/BatteryIndicator';
import NetworkIndicator from '../../components/system/NetworkIndicator';


export default function RobotStatus() {
    return (
        <Stack direction="row" spacing={1} alignItems="flex-end" justifyContent="flex-end" fontSize="large" >
            <SignalIndicator />
            <NetworkIndicator />
            <BatteryIndicator />
        </Stack>
    )
}