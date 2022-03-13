import { Box, Card, CardContent, CardHeader, Chip, Grid, LinearProgress, Stack, Switch, Tooltip } from '@mui/material';

import Page from '../components/Page';
import Channels from '../components/rcremote/Channels';
import { useGetRCReceiverQuery, useSetRCReceiverMutation } from '../services/rcreceiver';






export default function Receiver() {
    const { data: rc, isSuccess } = useGetRCReceiverQuery();
    const [ update ] = useSetRCReceiverMutation();


    const enableChanged = (event : React.ChangeEvent<HTMLInputElement>) => {
        update({ enabled: event.target.checked });
    }

    return (
        <Page>
            <Box padding={{ xs: 1, md: 2 }}>
                <Card>
                    <CardHeader 
                        title={
                            <Stack direction="row" alignItems="center" spacing={1}>
                                <span>Channels</span>
                                { isSuccess && !rc.enabled && <Chip size="small" color="warning" label="disabled"/>}
                                { isSuccess && rc.enabled && rc.connected && <Chip size="small" color="success" label="connected"/>}
                                { isSuccess && rc.enabled && !rc.connected && <Chip size="small" color="error" label="no signal"/>}
                            </Stack>
                        }
                        action={ isSuccess && <Switch checked={rc?.enabled || false} onChange={enableChanged} /> }
                    />
                    <CardContent>
                        { isSuccess && rc.enabled &&
                            <LinearProgress 
                                value={rc.rssi} 
                                variant={rc.connected?"determinate":"indeterminate"}
                            />
                        }
                    </CardContent>
                    <CardContent>
                        <Channels />
                    </CardContent>
                </Card>
            </Box>
        </Page>
    )
}