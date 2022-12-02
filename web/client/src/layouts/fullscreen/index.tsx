import { Outlet } from 'react-router-dom';
import { styled } from '@mui/material/styles';


const RootStyle = styled('div')(({ theme }) => ({
    height: '100vh',
    display: 'flex',
    overflow: "hidden",
}));


export default function FullscreenLayout() {
    return (
        <RootStyle>
            <Outlet />
        </RootStyle>
    );
}
