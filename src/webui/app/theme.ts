import {createTheme} from '@mui/material/styles';

const lightTheme = createTheme({
    palette: {
        mode: 'light',
        primary: {
            main: '#1c92b9',
            light: '#3eaccc',
        },
        secondary: {
            main: '#b9431c',
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
            main: '#1c92b9',
            light: '#3eaccc',
        },
        secondary: {
            main: '#b9431c',
            light: '#fa632e'
        },
    },
});

export { lightTheme, darkTheme };