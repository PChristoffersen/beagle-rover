import { Outlet } from 'react-router-dom';
import { styled } from '@mui/material/styles';


const RootStyle = styled('div')(({ theme }) => ({
}));


export default function FullscreenLayout() {
    return (
        <RootStyle>
            <Outlet />
        </RootStyle>
    );
}
