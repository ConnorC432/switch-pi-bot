import {createTheme} from '@mui/material/styles';

const lightTheme = createTheme({
    palette: {
        mode: 'light',
        primary: {
            main: '#0ab9e6',
            light: '#3eaccc',
        },
        secondary: {
            //main: '#b9431c',
            main: '#ff3c28',
            light: '#fa632e'
        },
        background: {
            default: '#f1eeee',
            paper: '#f1eeee',
        },
    },
});

const darkTheme = createTheme({
    palette: {
        mode: 'dark',
        primary: {
            main: '#0ab9e6',
            light: '#3eaccc',
        },
        secondary: {
            main: '#ff3c28',
            light: '#fa632e'
        },
    },
});

export { lightTheme, darkTheme };