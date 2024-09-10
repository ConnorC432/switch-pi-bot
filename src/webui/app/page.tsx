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

interface Program {
    id: number;
    name: string;
    settings: { [key: string]: any };
}

/*interface ProgramsData {
    [key: string]: Program[];
}*/

interface StatusResponse {
    status: 'Starting' | 'Running' | 'Error' | 'Finished' | 'Pending';
    currentGame?: {
        id: string;
        name: string;
    };
    currentProgram?: {
        id: number;
        name: string;
        settings: { [key: string]: any };
    };
}

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

    const formatKey = (key: string) => {
        return key
            .replace(/([a-z])([A-Z])/g, '$1 $2')  // Add space before capital letters
            .replace(/^./, (str) => str.toUpperCase());  // Capitalize the first letter
    }

    // Fetch JSON data for games/programs
    React.useEffect(() => {
        async function fetchData() {
            const response = await fetch('/api/programs/get-programs');
            if (!response.ok) {
                console.error('Failed to fetch programs.');
                return;
            }
            const data = await response.json();

            // Extract games from JSON
            const gameList = Object.keys(data);
            setGames(gameList);

            // Set programs based on selected game
            if (selectedGame) {
                setPrograms(data[selectedGame] || []);
            }
        }

        fetchData();
    }, [selectedGame]);

    // Poll the status every second
    React.useEffect(() => {
        const fetchStatus = async () => {
            const response = await fetch('/api/programs/get-program-status');
            if (response.ok) {
                const data: StatusResponse = await response.json();
                setStatus(data);
            } else {
                console.error('Failed to fetch status');
            }
        };

        fetchStatus(); // Initial fetch
        const interval = setInterval(fetchStatus, 1000); // Poll every second

        return () => clearInterval(interval); // Cleanup interval on component unmount
    }, []);

    // Update Capture image
    React.useEffect(() => {
        let isMounted = true;
        const interval = setInterval(() => {
            if (isMounted) {
                setCaptureSrc(`http://localhost:5000/video-stream?${Date.now()}`);
            }
        }, 500);

        return () => {
            clearInterval(interval)
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

    const saveSettings = async () => {
        if (selectedProgram) {
            const response = await fetch('/api/programs/save-settings', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    game: selectedGame,
                    programId: selectedProgram.id,
                    updatedSettings: settings,
                }),
            });

            if (response.ok) {
                console.log('Settings Saved');
            } else {
                console.log('Settings Not Saved');
            }
        }
    };

    const startProgram = async () => {
        if (selectedProgram) {
            setLoading(true);
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
            setLoading(false);
        }
    };

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
                                        backgroundColor: theme.palette.action.hover, // You can change this to any color you prefer
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
                                            backgroundColor: theme.palette.action.hover, // You can change this to any color you prefer
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
                                    disabled={loading} // Disable the button while loading
                                >
                                    {loading ? <CircularProgress size={24} /> : 'Start Program'}
                                </Button>
                            </Box>
                        </Box>
                    </AccordionDetails>
                </Accordion>
            )}
        </Box>
    );
}
