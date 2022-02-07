import BatteryUnknownIcon from '@mui/icons-material/BatteryUnknown';
import SignalWifiBadIcon from '@mui/icons-material/SignalWifiBad';
import { Stack } from '@mui/material';
import SignalIndicator from '../../components/rcremote/SignalIndicator';


export default function RobotStatus() {
    return (
        <Stack direction="row" spacing={1} alignItems="flex-end" justifyContent="flex-end">
            <SignalIndicator />
            <SignalWifiBadIcon fontSize='large' />
            <BatteryUnknownIcon fontSize='large' />
        </Stack>
    )
}