import { Button } from '@mui/material';
import { styled } from '@mui/material/styles';
import { Link as RouterLink } from 'react-router-dom';
import Page from "../components/Page";
import RestUI from "../components/rest/RestUI";
import OpenInFullIcon from '@mui/icons-material/OpenInFull';


const Root = styled('div')(({ theme }) => ({
    position: "relative",
}));

const Body = styled('div')(({ theme }) => ({
    position: "absolute",
    width: '100%',
    top: 0,
}));


const Controls = styled('div')(({ theme }) => ({
    position: "absolute",
    right: 0,
    top: 0,
    zIndex: 300,
}));


export default function RestAPI() {
    return (
        <Page sx={{ padding: { xs: 1, sm: 2 }}}>
            <Root>
                <Body>
                    <RestUI />
                </Body>
                <Controls>
                    <Button
                        component={RouterLink}
                        to="/rest-full"
                    >
                        <OpenInFullIcon fontSize='medium'/>
                    </Button>
                </Controls>
            </Root>
        </Page>
    )
}