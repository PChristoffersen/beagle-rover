import * as React from 'react';
import { Link as RouterLink, useLocation, matchPath } from 'react-router-dom';
import AppBar from '@mui/material/AppBar';
import Box from '@mui/material/Box';
import Toolbar from '@mui/material/Toolbar';
import IconButton from '@mui/material/IconButton';
import Typography from '@mui/material/Typography';
import Menu from '@mui/material/Menu';
import MenuIcon from '@mui/icons-material/Menu';
import Container from '@mui/material/Container';
import Button from '@mui/material/Button';
import MenuItem from '@mui/material/MenuItem';

import { pages } from '../../Router';
import Logo from '../../components/Logo';
import RobotStatus from './RobotStatus';
import { Tab, Tabs, useTheme } from '@mui/material';



export default function Navbar() {
    const { palette } = useTheme();
    const { pathname } = useLocation();
    const currentPage = pages.find((page) => !!matchPath(page.to + "/*", pathname))

    const [anchorElNav, setAnchorElNav] = React.useState<null | HTMLElement>(null);
  
    console.log(palette);

    const handleOpenNavMenu = (event: React.MouseEvent<HTMLElement>) => {
      setAnchorElNav(event.currentTarget);
    };
  
    const handleCloseNavMenu = () => {
      setAnchorElNav(null);
    };
  
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
                        <Logo />
                    </Typography>
                    <Box sx={{ flexGrow: 1, display: { xs: 'flex', md: 'none' } }}>
                        <IconButton
                            size="large"
                            aria-label="account of current user"
                            aria-controls="menu-appbar"
                            aria-haspopup="true"
                            onClick={handleOpenNavMenu}
                            color="inherit"
                        >
                            <MenuIcon />
                        </IconButton>
                        <Menu
                            id="menu-appbar"
                            anchorEl={anchorElNav}
                            anchorOrigin={{
                                vertical: 'bottom',
                                horizontal: 'left',
                            }}
                            keepMounted
                            transformOrigin={{
                                vertical: 'top',
                                horizontal: 'left',
                            }}
                            open={Boolean(anchorElNav)}
                            onClose={handleCloseNavMenu}
                            sx={{
                                display: { xs: 'block', md: 'none' },
                            }}
                        >
                            {pages.map((page) => (
                                <MenuItem key={page.name} component={RouterLink} to={page.to} onClick={handleCloseNavMenu}>
                                <Typography textAlign="center">{page.name}</Typography>
                                </MenuItem>
                            ))}
                        </Menu>
                    </Box>
                    <Typography
                        variant="h6"
                        noWrap
                        component="div"
                        sx={{ flexGrow: 1, display: { xs: 'flex', md: 'none' } }}
                    >
                        <Logo />
                    </Typography>
                    {false && <Box sx={{ flexGrow: 1, display: { xs: 'none', md: 'flex' } }}>
                        <Tabs value={currentPage?.id}>
                            {pages.map((page) => (
                                <Tab
                                    key={page.name}
                                    component={RouterLink}
                                    to={page.to}
                                    label={page.name}
                                    value={page.id}
                                />
                            ))}
                        </Tabs>
                    </Box>}
                    {true && <Box sx={{ flexGrow: 1, display: { xs: 'none', md: 'flex' } }}>
                        {pages.map((page) => (
                            <Button
                                key={page.name}
                                component={RouterLink}
                                to={page.to}
                                color={page.id===currentPage?.id ?'inherit': undefined}
                            >
                                {page.name}
                            </Button>
                        ))}
                    </Box>}

                    <Box sx={{ flexGrow: 0, verticalAlign: 'bottom' }}>
                        <RobotStatus />
                    </Box>
                </Toolbar>
            </Container>
        </AppBar>
    );
}
