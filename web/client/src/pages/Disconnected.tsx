import { Box, Typography } from "@mui/material";
import { ConnectionState } from "../features/backendSlice";
import { useAppSelector } from "../hooks";

export default function Disconnected() {
    const connection = useAppSelector((state) => state.backend.connection);

    //console.log("Yip");

    return (
        <Box>
            <Typography variant="h6">
                {connection}
                {connection===ConnectionState.CONNECTED && <div>connected</div>}
                {connection===ConnectionState.CONNECTING && <div>Connecting...</div>}
            </Typography>
        </Box>
    )
}