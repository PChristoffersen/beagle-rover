import { Box, Typography } from "@mui/material";
import { styled } from '@mui/material/styles';
import Page from "../components/Page";
import RestUI from "../components/rest/RestUI";

export default function RestAPI() {
    return (
        <Page sx={{ padding: { xs: 1, sm: 2 } }}>
            <RestUI />
        </Page>
    )
}