import * as React from 'react';
import { AppBar, Container, Toolbar, Typography, Box, Menu, MenuItem, IconButton, Tooltip, Avatar, Button } from '@mui/material';

const pages = ['Products', 'Pricing', 'Blog'];


export default function topBar() {
    //const [anchorElNav, setAnchorElNav] = React.useState<null | HTMLElement>(null);
    //const [anchorElUser, setAnchorElUser] = React.useState<null | HTMLElement>(null);

    /*
    const handleOpenNavMenu = (event: React.MouseEvent<HTMLElement>) => {
        setAnchorElNav(event.currentTarget);
    };
    const handleOpenUserMenu = (event: React.MouseEvent<HTMLElement>) => {
        setAnchorElUser(event.currentTarget);
    };

    const handleCloseNavMenu = () => {
        setAnchorElNav(null);
    };

    const handleCloseUserMenu = () => {
        setAnchorElUser(null);
    };
    */

    return (
        <AppBar position="static">
        <Container maxWidth="xl">
            <Toolbar disableGutters>
            <Typography
                variant="h6"
                noWrap
                component="div"
                sx={{ mr: 2, display: { xs: 'none', md: 'flex' } }}
            >
                LOGO
            </Typography>

            <Typography
                variant="h6"
                noWrap
                component="div"
                sx={{ flexGrow: 1, display: { xs: 'flex', md: 'none' } }}
            >
                LOGO
            </Typography>
            <Box sx={{ flexGrow: 1, display: { xs: 'none', md: 'flex' } }}>
                {pages.map((page) => (
                <Button
                    key={page}
                    sx={{ my: 2, color: 'white', display: 'block' }}
                >
                    {page}
                </Button>
                ))}
            </Box>

            </Toolbar>
        </Container>
        </AppBar>
    );
};
