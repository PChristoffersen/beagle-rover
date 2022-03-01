import { Outlet } from 'react-router-dom';
import { styled } from '@mui/material/styles';

import Navbar from './Navbar';

const RootStyle = styled('div')(({ theme }) => ({
    height: '100vh',
    display: 'flex',
    flexFlow: 'column',
}));

const HeaderStyle = styled('div')(({ theme }) => ({
    flex: '0 1 auto',
}));


const BodyStyle = styled('div')(({ theme }) => ({
    flex: '1 1 auto',
}));


export default function MainLayout() {
    return (
        <RootStyle id='LayoutRoot'>
            <HeaderStyle id="LayoutHeader">
                <Navbar />
            </HeaderStyle>
            <BodyStyle id="LayoutBody">
                <Outlet />
            </BodyStyle>
        </RootStyle>
    );
}