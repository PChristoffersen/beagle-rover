import { Grid, LinearProgress, Slider, Stack, Table, TableBody, TableCell, TableHead, TableRow, useMediaQuery, useTheme } from "@mui/material";
import { range } from "lodash";
import { RCChannels } from "../../services/model";
import { useGetRCChannelsQuery } from "../../services/rcreceiver";
import VariableProgress from "../common/VariableProgress";


interface ChannelTableProps {
    startIndex: number;
    endIndex: number;
    channels: RCChannels;
}

function ChannelTable({ startIndex, endIndex, channels }: ChannelTableProps) {
    return (
        <Table>
            <TableBody>
            { range(startIndex, endIndex).map((index) => (
                <TableRow key={index}>
                    <TableCell align="right" width="fit-content">{index}</TableCell>
                    <TableCell align="right" width="fit-content">{channels[index]}</TableCell>
                    <TableCell width="100%"><VariableProgress value={channels[index]} min={500} max={2500} height={16} /></TableCell>
                </TableRow>
            ))}
            </TableBody>
        </Table>
    )
}


export default function Channels() {
    const theme = useTheme();
    const isSmall = useMediaQuery(theme.breakpoints.down('sm'))
    const { data: channels } = useGetRCChannelsQuery();

    if (isSmall) {
        return (
            <>
                { channels && <ChannelTable startIndex={0} endIndex={channels.length} channels={channels} />}
            </>
        );
    }
    else {
        return (
            <Grid container spacing={3}>
                {channels && <>
                    <Grid item xs={6}>
                        <ChannelTable startIndex={0} endIndex={channels.length/2} channels={channels} />
                    </Grid>
                    <Grid item xs={6}>
                        <ChannelTable startIndex={channels.length/2} endIndex={channels.length} channels={channels} />
                    </Grid>
                </>}
            </Grid>
        );
    }
}