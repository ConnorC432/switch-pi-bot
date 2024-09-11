// Page.tsx

import * as React from 'react';
import {
    Box,
    Typography,
    Accordion,
    AccordionDetails,
    AccordionSummary,
    List,
    ListItem,
    ListItemText,
    TextField,
    Button,
    useTheme,
    CircularProgress
} from '@mui/material';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';
import type { SelectChangeEvent } from '@mui/material';
import { ProgramService, Program, StatusResponse } from './services/ProgramService';

export default function Page() {
    const [selectedGame, setSelectedGame] = React.useState<string>('');
    const [selectedProgram, setSelectedProgram] = React.useState<Program | null>(null);
    const [programs, setPrograms] = React.useState<Program[]>([]);
    const [games, setGames] = React.useState<string[]>([]);
    const [settings, setSettings] = React.useState<{ [key: string]: any }>({});
    const [status, setStatus] = React.useState<StatusResponse | null>(null);
    const [loading, setLoading] = React.useState(false);
    const [captureSrc, setCaptureSrc] = React.useState<string>(`http://localhost:5000/video-stream?${Date.now()}`);

    const theme = useTheme();
    const programService = new ProgramService();

    const formatKey = (key: string) => {
        return key
            .replace(/([a-z])([A-Z])/g, '$1 $2')
            .replace(/^./, (str) => str.toUpperCase());
    };

    // Fetch programs and games
    React.useEffect(() => {
        async function fetchData() {
            try {
                const data = await programService.fetchPrograms();
                const gameList = Object.keys(data);
                setGames(gameList);

                if (selectedGame) {
                    setPrograms(data[selectedGame] || []);
                }
            } catch (error) {
                console.error(error);
            }
        }

        fetchData();
    }, [selectedGame]);

    // Poll the status every 5 seconds
    React.useEffect(() => {
        const fetchStatus = async () => {
            try {
                const data = await programService.fetchStatus();
                setStatus(data);
            } catch (error) {
                console.error(error);
            }
        };

        fetchStatus();
        const interval = setInterval(fetchStatus, 5000);

        return () => clearInterval(interval);
    }, []);

    // Update Capture image
    React.useEffect(() => {
        let isMounted = true;
        const interval = setInterval(() => {
            if (isMounted) {
                setCaptureSrc(`http://localhost:5000/video-stream?${Date.now()}`);
            }
        }, 250);

        return () => {
            clearInterval(interval);
            isMounted = false;
        };
    }, []);

    // Handle game change
    const handleGameChange = (event: SelectChangeEvent<string>) => {
        const game = event.target.value as string;
        setSelectedGame(game);
        setSelectedProgram(null);
        setSettings({});
    };

    // Handle program change
    const handleProgramChange = (program: Program) => {
        setSelectedProgram(program);
        setSettings(program.settings || {});
    };

    // Handle setting change
    const handleSettingChange = (key: string) => (event: React.ChangeEvent<HTMLInputElement>) => {
        setSettings({
            ...settings,
            [key]: event.target.value,
        });
    };

    // Save program settings
    const saveSettings = async () => {
        if (selectedProgram) {
            try {
                await programService.saveSettings(selectedGame, selectedProgram.id, settings);
                console.log('Settings Saved');
            } catch (error) {
                console.log('Settings Not Saved');
            }
        }
    };

    // Start the program
    const startProgram = async () => {
        if (selectedProgram) {
            setLoading(true);
            try {
                const data = await programService.startProgram(selectedGame, selectedProgram.id);
                setStatus(data);
            } catch (error) {
                console.error('Failed to start program');
            } finally {
                setLoading(false);
            }
        }
    };

    const isStatusStartingOrRunning = status && (status.status === 'Starting' || status.status === 'Running');
    const buttonDisabled = loading || isStatusStartingOrRunning;

    return (
        <Box
            sx={{
                display: 'flex',
                justifyContent: 'center',
                flexDirection: 'column',
                alignItems: 'center',
                mt: '64px',
                width: '100%',
                maxWidth: 1200,
                mx: 'auto',
                px: 2
            }}
        >
            {/* Video Stream */}
            <Box
                sx={{
                    position: 'relative',
                    width: '100%',
                    height: 0,
                    paddingBottom: '56.25%',
                    overflow: 'hidden',
                    borderRadius: 1,
                    boxShadow: 3,
                    backgroundColor: 'theme.palette.background.paper',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                }}
            >
                <img src={captureSrc}
                     alt="Capture Card Unavailable"
                     style={{
                         position: 'absolute',
                         top: 0,
                         left: 0,
                         width: '100%',
                         height: '100%',
                         objectFit: 'fill'
                }}/>
            </Box>

            {/* Remaining JSX... */}
        </Box>
    );
}
