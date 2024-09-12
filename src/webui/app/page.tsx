"use client";

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
import { JSONInterface, Program, StatusResponse } from './json';

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
    const jsonInterface = new JSONInterface();

    const formatKey = (key: string) => {
        return key
            .replace(/([a-z])([A-Z])/g, '$1 $2')
            .replace(/^./, (str) => str.toUpperCase());
    };

    // Fetch programs and games
    React.useEffect(() => {
        async function fetchData() {
            try {
                const data = await jsonInterface.fetchPrograms();
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

    /*
    React.useEffect(() => {
        async function initAccordion() {
            try {
                const initStatus = await jsonInterface.fetchStatus();
                const currentGameName = initStatus.currentGame?.name || '';

                // Ensure handleGameChange is called with the correct format
                handleGameChange({ target: { value: currentGameName } } as SelectChangeEvent<string>);

                const currentProgram = initStatus.currentProgram || null;
                if (currentProgram) {
                    // Handle program change with the correct object type
                    handleProgramChange(currentProgram);

                    // Update settings with the current program settings
                    const currentProgramSettings = currentProgram.settings || {};
                    setSettings(currentProgramSettings);
                } else {
                    // Clear settings if no program
                    setSettings({});
                }
            } catch (error) {
                console.error('Failed to initialize accordion:', error);
            }
        }

        initAccordion();
    }, []);*/

    // Poll the status every 5 seconds
    React.useEffect(() => {
        const fetchStatus = async () => {
            try {
                const data = await jsonInterface.fetchStatus();
                setStatus(data);

                // Update selectedGame and selectedProgram based on status
                /*if (data.status === 'Running') {
                    setSelectedGame(data.currentGame?.name || '');
                    setSelectedProgram(data.currentProgram || null);
                }*/
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
        setSelectedProgram(null); // Reset selected program when game changes
        setSettings({}); // Clear settings when game changes
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
            const confirmSave = window.confirm('Are you sure you want to save the settings?');
            if (confirmSave) {
                try {
                    await jsonInterface.saveSettings(selectedGame, selectedProgram.id, settings);
                    console.log('Settings Saved');
                } catch (error) {
                    console.log('Settings Not Saved');
                }
            }
        }
    };

    // Start program
    const startProgram = async () => {
        if (selectedProgram) {
            setLoading(true);
            try {
                const response = await fetch('http://localhost:5000/start-program', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({
                        game: selectedGame,
                        programId: selectedProgram.id,
                    }),
                });

                if (response.ok) {
                    const data: StatusResponse = await response.json();
                    setStatus(data);
                } else {
                    console.error('Failed to start program');
                }
            } catch (error) {
                console.error('Failed to start program', error);
            }
            setLoading(false);
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
                    backgroundColor: theme.palette.background.paper,
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

            {/* Status Display */}
            {status && (status.status === 'Starting' || status.status === 'Running' || status.status === 'Error' || status.status === 'Finished') && (
                <Box sx={{ mt: 2, width: '100%', textAlign: 'center' }}>
                    <Typography variant="h6" color="textSecondary">
                        Current Status: {status.status}
                    </Typography>
                    {status.currentGame && (
                        <Typography variant="body1" color="textSecondary">
                            Game: {status.currentGame.name}
                        </Typography>
                    )}
                    {status.currentProgram && (
                        <Typography variant="body1" color="textSecondary">
                            Program: {status.currentProgram.name}
                        </Typography>
                    )}
                </Box>
            )}

            {/* Game Selection Accordion */}
            <Accordion sx={{ mt: 2, width: '100%' }}>
                <AccordionSummary
                    expandIcon={<ExpandMoreIcon />}
                    aria-controls="game-selection-content"
                    id="game-selection-header"
                >
                    <Typography>{selectedGame || 'Select Game'}</Typography>
                </AccordionSummary>
                <AccordionDetails>
                    <List>
                        {games.map((game) => (
                            <ListItem
                                key={game}
                                onClick={() => handleGameChange({ target: { value: game } } as SelectChangeEvent<string>)}
                                sx={{
                                    cursor: 'pointer',
                                    '&:hover': {
                                        backgroundColor: theme.palette.action.hover,
                                    },
                                }}
                            >
                                <ListItemText primary={game} />
                            </ListItem>
                        ))}
                    </List>
                </AccordionDetails>
            </Accordion>

            {/* Program Selection Accordion */}
            {selectedGame && (
                <Accordion sx={{ mt: 2, width: '100%' }}>
                    <AccordionSummary
                        expandIcon={<ExpandMoreIcon />}
                        aria-controls="program-selection-content"
                        id="program-selection-header"
                    >
                        <Typography>{selectedProgram ? selectedProgram.name : 'Select Program'}</Typography>
                    </AccordionSummary>
                    <AccordionDetails>
                        <List>
                            {programs.map((program) => (
                                <ListItem
                                    key={program.id}
                                    onClick={() => handleProgramChange(program)}
                                    sx={{
                                        cursor: 'pointer',
                                        '&:hover': {
                                            backgroundColor: theme.palette.action.hover,
                                        },
                                    }}
                                >
                                    <ListItemText primary={program.name} />
                                </ListItem>
                            ))}
                        </List>
                    </AccordionDetails>
                </Accordion>
            )}

            {/* Program Settings Accordion */}
            {selectedProgram && (
                <Accordion sx={{ mt: 2, width: '100%' }}>
                    <AccordionSummary
                        expandIcon={<ExpandMoreIcon />}
                        aria-controls="program-settings-content"
                        id="program-settings-header"
                    >
                        <Typography>Program Settings for {selectedProgram.name}</Typography>
                    </AccordionSummary>
                    <AccordionDetails>
                        <Box sx={{ width: '100%' }}>
                            {Object.keys(selectedProgram.settings).map((key) => (
                                <TextField
                                    key={key}
                                    fullWidth
                                    label={formatKey(key)}
                                    value={settings[key] || ''}
                                    onChange={handleSettingChange(key)}
                                    sx={{ mb: 2 }}
                                />
                            ))}

                            {/* Button Container */}
                            <Box sx={{ display: 'flex', gap: 2 }}>
                                <Button
                                    variant="contained"
                                    color="primary"
                                    onClick={saveSettings}
                                >
                                    Save Settings
                                </Button>
                                <Button
                                    variant="contained"
                                    color="secondary"
                                    onClick={startProgram}
                                    sx={{ display: 'flex', alignItems: 'center' }}
                                    disabled={buttonDisabled}
                                >
                                    {buttonDisabled ? <CircularProgress size={24} /> : 'Start Program'}
                                </Button>
                            </Box>
                        </Box>
                    </AccordionDetails>
                </Accordion>
            )}
        </Box>
    );
}
