"use client";

import * as React from 'react';
import {
    Box,
    Typography,
    Accordion,
    AccordionDetails,
    AccordionSummary,
    TextField,
    Button,
    useTheme,
    CircularProgress
} from '@mui/material';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';

interface Setting {
    name: string;
    type: 'text' | 'dropdown' | 'combo';
    value: string;
    options?: string[];
}

interface SettingsGroup {
    [key: string]: Setting[];
}

export default function Page() {
    const [settingsGroups, setSettingsGroups] = React.useState<SettingsGroup>({});
    const [updatedSettings, setUpdatedSettings] = React.useState<{ [key: string]: string }>({});
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState<string | null>(null);
    const theme = useTheme();

    // Fetch settings data from API
    React.useEffect(() => {
        const fetchSettings = async () => {
            try {
                const response = await fetch('/api/settings');
                if (!response.ok) {
                    throw new Error('Failed to fetch settings');
                }
                const data: SettingsGroup = await response.json();
                setSettingsGroups(data);

                // Flatten settings to update the settings state
                const flattenedSettings = Object.entries(data).reduce((acc, [group, settings]) => {
                    settings.forEach(setting => {
                        acc[`${group}.${setting.name}`] = setting.value;
                    });
                    return acc;
                }, {} as { [key: string]: string });

                setUpdatedSettings(flattenedSettings);
            } catch (error) {
                setError(error.message);
            } finally {
                setLoading(false);
            }
        };

        fetchSettings();
    }, []);

    // Handle setting change for text fields
    const handleTextSettingChange = (group: string, name: string) => (event: React.ChangeEvent<HTMLInputElement>) => {
        setUpdatedSettings(prevSettings => ({
            ...prevSettings,
            [`${group}.${name}`]: event.target.value,
        }));
    };

    // Handle setting change for select fields
    const handleSelectSettingChange = (group: string, name: string) => (event: React.ChangeEvent<HTMLSelectElement>) => {
        setUpdatedSettings(prevSettings => ({
            ...prevSettings,
            [`${group}.${name}`]: event.target.value,
        }));
    };

    // Save settings
    const saveSettings = async () => {
        const confirmSave = window.confirm('Are you sure you want to save the settings?');
        if (!confirmSave) return;

        try {
            // Group settings by their respective group with type included
            const groupedSettings: SettingsGroup = Object.entries(updatedSettings).reduce((acc, [key, value]) => {
                const [group, settingName] = key.split('.');
                if (!acc[group]) {
                    acc[group] = [];
                }
                const setting = settingsGroups[group].find(setting => setting.name === settingName);
                if (setting) {
                    acc[group].push({
                        ...setting,
                        value: value
                    });
                }
                return acc;
            }, {} as SettingsGroup);

            const response = await fetch('/api/settings', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(groupedSettings),
            });

            if (!response.ok) {
                throw new Error('Failed to save settings');
            }

            window.location.reload()
        } catch (error) {
            alert('Error saving settings: ' + error.message);
        }
    };

    if (loading) {
        return (
            <Box
                sx={{
                    display: 'flex',
                    justifyContent: 'center',
                    alignItems: 'center',
                    height: '100vh',
                }}
            >
                <CircularProgress />
            </Box>
        );
    }

    if (error) {
        return (
            <Box
                sx={{
                    display: 'flex',
                    justifyContent: 'center',
                    alignItems: 'center',
                    height: '100vh',
                }}
            >
                <Typography color="error">{error}</Typography>
            </Box>
        );
    }

    return (
        <Box
            sx={{
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'center',
                mt: 4,
                width: '100%',
                maxWidth: 1200,
                mx: 'auto',
                px: 2
            }}
        >

            {Object.keys(settingsGroups).map((group) => (
                <Accordion key={group} sx={{ mt: 2, width: '100%' }}>
                    <AccordionSummary
                        expandIcon={<ExpandMoreIcon />}
                        aria-controls={`${group}-content`}
                        id={`${group}-header`}
                    >
                        <Typography>{group}</Typography>
                    </AccordionSummary>
                    <AccordionDetails>
                        {settingsGroups[group].map((setting) => (
                            <Box
                                key={setting.name}
                                sx={{
                                    display: 'flex',
                                    alignItems: 'center',
                                    width: '100%',
                                    gap: 2,
                                    mb: 2
                                }}
                            >
                                <Typography
                                    variant="body1"
                                    sx={{ width: '200px', fontWeight: 'bold' }}
                                >
                                    {setting.name}
                                </Typography>
                                {setting.type === 'text' && (
                                    <TextField
                                        fullWidth
                                        value={updatedSettings[`${group}.${setting.name}`] || ''}
                                        onChange={handleTextSettingChange(group, setting.name)}
                                        sx={{ flexGrow: 1 }}
                                    />
                                )}
                                {setting.type === 'dropdown' && setting.options && (
                                    <TextField
                                        select
                                        value={updatedSettings[`${group}.${setting.name}`] || ''}
                                        onChange={handleSelectSettingChange(group, setting.name)}
                                        sx={{ flexGrow: 1 }}
                                        SelectProps={{
                                            native: true,
                                        }}
                                    >
                                        {setting.options.map((option) => (
                                            <option key={option} value={option}>
                                                {option}
                                            </option>
                                        ))}
                                    </TextField>
                                )}
                                {setting.type === 'combo' && setting.options && (
                                    <TextField
                                        select
                                        value={updatedSettings[`${group}.${setting.name}`] || ''}
                                        onChange={handleSelectSettingChange(group, setting.name)}
                                        sx={{ flexGrow: 1 }}
                                        SelectProps={{
                                            native: true,
                                        }}
                                    >
                                        {setting.options.map((option) => (
                                            <option key={option} value={option}>
                                                {option}
                                            </option>
                                        ))}
                                    </TextField>
                                )}
                            </Box>
                        ))}
                    </AccordionDetails>
                </Accordion>
            ))}

            <Button
                variant="contained"
                color="primary"
                onClick={saveSettings}
                sx={{ mt: 4 }}
            >
                Save Settings
            </Button>
        </Box>
    );
}
