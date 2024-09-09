"use client";

import * as React from 'react';
import {
    Drawer,
    IconButton,
    List,
    ListItem,
    ListItemButton,
    ListItemText,
    CssBaseline,
    Box,
    Divider,
    Toolbar,
    Typography,
    useTheme,
    Paper
} from '@mui/material';
import MenuIcon from '@mui/icons-material/Menu';
import Link from 'next/link';
import { ThemeProvider } from '@mui/material/styles';
import { lightTheme, darkTheme } from './theme';

const drawerWidth = 240;

const menuItems = [
    { label: 'Switch', href: '/' },
    { label: 'Settings', href: '/settings' },
];

export default function RootLayout({ children }: { children: React.ReactNode }) {
    const [isDarkMode, setDarkMode] = React.useState(false);
    const handleThemeToggle = () => {
        setDarkMode(!isDarkMode);
    };

    const [mobileOpen, setMobileOpen] = React.useState(false);

    const handleDrawerToggle = () => {
        setMobileOpen(!mobileOpen);
    };

    const theme = useTheme(); // Use the theme

    const drawer = (
        <div>
            <Toolbar />
            <Divider />
            <List>
                {menuItems.map((item, index) => (
                    <ListItem key={index} disablePadding>
                        <ListItemButton component={Link} href={item.href}>
                            <ListItemText primary={item.label} />
                        </ListItemButton>
                    </ListItem>
                ))}
            </List>
            <Divider />
        </div>
    );

    return (
        <html lang="en">
        <head>
            <title>Nintendo Switch Automation UI</title>
            <meta name="viewport" content="width=device-width, initial-scale=1" />
        </head>
        <body>
        <ThemeProvider theme={isDarkMode ? darkTheme : lightTheme}>
            <CssBaseline />
            <Box sx={{ display: 'flex' }}>
                {/* Header */}
                <Paper
                    elevation={4}
                    square
                    sx={{
                        position: 'fixed',
                        top: 0,
                        left: 0,
                        right: 0,
                        height: '64px',
                        backgroundColor: theme.palette.primary.main,
                        color: 'white',
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'space-between',
                        paddingX: 2,
                        zIndex: 1300, // Ensure header is above everything
                    }}
                >
                    <IconButton
                        color="inherit"
                        aria-label="open drawer"
                        onClick={handleDrawerToggle}
                        edge="start"
                        sx={{ zIndex: 1301, mr: 2 }} // Ensure button is above the header
                    >
                        <MenuIcon />
                    </IconButton>
                    <Box
                        sx={{
                            flexGrow: 1,
                            display: 'flex',
                            justifyContent: 'center'
                        }}
                    >
                        <Typography variant="h6">Pi Bot</Typography>
                    </Box>
                </Paper>

                {/* Drawer */}
                <Drawer
                    open={mobileOpen}
                    sx={{
                        width: drawerWidth,
                        flexShrink: 0,
                        '& .MuiDrawer-paper': {
                            width: drawerWidth,
                            boxSizing: 'border-box',
                            zIndex: 1200, // Ensure drawer is below header but above content
                            backgroundColor: theme.palette.background.paper,
                        },
                    }}
                >
                    {drawer}
                </Drawer>

                {/* Main content area */}
                <Box
                    component="main"
                    sx={{
                        flexGrow: 1,
                        p: 3,
                        ml: mobileOpen ? `${drawerWidth}px` : 0, // Margin left when drawer is open
                        mt: '64px', // Offset for the sticky header
                        maxWidth: '1200px', // Optional: Maximum width to constrain content
                        mx: 'auto', // Center horizontally
                        transition: 'margin 0.3s ease', // Smooth transition when opening/closing drawer
                    }}
                >
                    {children} {/* Render the content of the current tab */}
                </Box>
            </Box>
        </ThemeProvider>
        </body>
        </html>
    );
}
