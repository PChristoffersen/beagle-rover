import { Outlet } from 'react-router-dom';
import { styled } from '@mui/material/styles';

import Navbar from './Navbar';

const APP_BAR_MOBILE = 64;
const APP_BAR_DESKTOP = 92;

const RootStyle = styled('div')(({ theme }) => ({
    /*
    display: 'flex',
    flex: '1 1 auto',
    maxWidth: '100%',
    paddingTop: 64,
    [theme.breakpoints.up('lg')]: {
        paddingLeft: 280
    }*/
}));

/*
const MainStyle = styled('div')(({ theme }) => ({
    flexGrow: 1,
    overflow: 'auto',
    minHeight: '100%',
    paddingTop: APP_BAR_MOBILE + 24,
    paddingBottom: theme.spacing(10),
    [theme.breakpoints.up('lg')]: {
        paddingTop: APP_BAR_DESKTOP + 24,
        paddingLeft: theme.spacing(2),
        paddingRight: theme.spacing(2)
    }
}));
*/


export default function MainLayout() {
    return (
        <RootStyle>
            <Navbar />
            <Outlet />
        </RootStyle>
    );
}