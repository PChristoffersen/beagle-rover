import { styled } from '@mui/material/styles';
import { Link as RouterLink } from 'react-router-dom';
import { Button } from '@mui/material';
import CloseFullscreenIcon from '@mui/icons-material/CloseFullscreen';
import RestUIStock from "../components/rest/RestUIStock";


const PageStyle = styled('div')(({ theme }) => ({
    height: '100%',
    width: '100%',
}));


const Root = styled('div')(({ theme }) => ({
    height: '100%',
    width: '100%',
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
    bottom: 0,
    zIndex: 300,
}));


export default function FullscreenRestAPI() {
    return (
        <PageStyle>
            <Root>
                <Body>
                    <RestUIStock />
                </Body>
                <Controls>
                    <Button
                        component={RouterLink}
                        to="/rest"
                    >
                        <CloseFullscreenIcon fontSize='large'/>
                    </Button>
                </Controls>
            </Root>
        </PageStyle>
    )
}