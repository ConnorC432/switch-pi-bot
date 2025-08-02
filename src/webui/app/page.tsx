"use client";

import * as React from "react";
import {
	Box,
	TextField,
	Button,
	useTheme,
	CircularProgress,
	FormControl,
	InputLabel,
	Select,
	MenuItem,
	List,
	ListItem,
	ListItemText,
	SelectChangeEvent,
} from "@mui/material";
import {JSONInterface, Program, StatusResponse} from "./json";
import StatusDisplay from "@/app/components/StatusDisplay";

// Define SettingValue to handle different types of setting values
type SettingValue = string | number | boolean;

export default function Page() {
	const [hostUrl, setHostUrl] = React.useState<string>("");
	const [selectedGame, setSelectedGame] = React.useState<string>("");
	const [selectedProgram, setSelectedProgram] = React.useState<Program | null>(null);
	const [programs, setPrograms] = React.useState<Program[]>([]);
	const [games, setGames] = React.useState<string[]>([]);
	const [settings, setSettings] = React.useState<{ [key: string]: SettingValue }>({});
	const [status, setStatus] = React.useState<StatusResponse | null>(null);
	const [loading, setLoading] = React.useState(false);
	const [captureSrc, setCaptureSrc] = React.useState<string>("");
	const theme = useTheme();
	const jsonInterface = React.useMemo(() => new JSONInterface(), []);
	const formatKey = (key: string) => {
		return key
			.replace(/([a-z])([A-Z])/g, "$1 $2")
			.replace(/^./, (str) => str.toUpperCase());
	};

	React.useEffect(() => {
		if (typeof window !== "undefined") {
			const host = window.location.hostname;
			setHostUrl(host);
		}
	}, []);

	React.useEffect(() => {
		if (hostUrl) {
			setCaptureSrc(`http://${hostUrl}:8080/stream`);
		}
	}, [hostUrl]);

	// Fetch programs and games
	React.useEffect(() => {
		async function fetchData() {
			try {
				const data = await jsonInterface.fetchPrograms();
				const gameList = Object.keys(data);
				setGames(gameList);

				// Check if the selected game is available in the fetched data
				if (selectedGame && gameList.includes(selectedGame)) {
					setPrograms(data[selectedGame] || []);
					setSelectedProgram(null); // Reset selected program when game changes
					setSettings({}); // Clear settings when game changes
				}
			}
			catch (error) {
				console.error("Failed to fetch programs and games:", error);
			}
		}

		fetchData();
	}, [selectedGame, jsonInterface]);

	// Handle game change
	const handleGameChange = (event: SelectChangeEvent<string>) => {
		const game = event.target.value;
		setSelectedGame(game);
		setSelectedProgram(null); // Reset selected program when game changes
		setSettings({}); // Clear settings when game changes
	};

	// Handle program selection
	const handleProgramSelect = (program: Program) => {
		setSelectedProgram(program);
		setSettings(program.settings || {});
	};

	// Handle setting change
	const handleSettingChange = (key: string) => (event: React.ChangeEvent<HTMLInputElement>) => {
		const value =
			event.target.type === "number"
				? Number(event.target.value)
				: event.target.type === "checkbox"
					? event.target.checked
					: event.target.value;
		setSettings({
			...settings,
			[key]: value,
		});
	};

	// Save program settings
	const saveSettings = async () => {
		if (selectedProgram) {
			const confirmSave = window.confirm("Are you sure you want to save the settings?");
			if (confirmSave) {
				try {
					await jsonInterface.saveSettings(selectedGame, selectedProgram.id, settings);
					console.log("Settings Saved");
				}
				catch (error) {
					console.log("Settings Not Saved:", error);
				}
			}
		}
	};

	// Start program
	const startProgram = async () => {
		if (selectedProgram) {
			setLoading(true);
			try {
				const response = await fetch(`http://${hostUrl}:5000/start-program`, {
					method: "POST",
					headers: {
						"Content-Type": "application/json",
					},
					body: JSON.stringify({
						game: selectedGame,
						programId: selectedProgram.id,
					}),
				});

				if (response.ok) {
					const data: StatusResponse = await response.json();
					setStatus(data);
				}
				else {
					console.error("Failed to start program");
				}
			}
			catch (error) {
				console.error("Failed to start program", error);
			}
			setLoading(false);
		}
	};

	return (
		<Box
			sx={{
				display: "flex",
				justifyContent: "center",
				flexDirection: "column",
				alignItems: "center",
				mt: "16px",
				width: "100%",
				maxWidth: 1200,
				mx: "auto",
				px: 2,
			}}
		>
			<Box sx={{ width: "100%", mb: 3 }}>
				{/* Video Stream */}
				<Box
					sx={{
						position: "relative",
						width: "100%",
						height: 0,
						paddingBottom: "56.25%",
						overflow: "hidden",
						borderRadius: 1,
						boxShadow: 3,
						backgroundColor: theme.palette.background.paper,
						display: "flex",
						alignItems: "center",
						justifyContent: "center",
					}}
				>
					<img
						src={captureSrc}
						alt="Capture Card Unavailable"
						style={{
							position: "absolute",
							top: 0,
							left: 0,
							width: "100%",
							height: "100%",
							objectFit: "cover",
						}}
						loading="eager"
					/>
				</Box>

				{/* Status Display */}
				<Box
					sx={{
						display: "flex",
						justifyContent: "center",
						alignItems: "center",
						mt: 2,
						width: "100%",
					}}
				>
					<StatusDisplay />
				</Box>

				{/* Buttons */}
				<Box sx={{ display: "flex", gap: 2, justifyContent: "center", mt: 2 }}>
					<Button variant="contained" color="primary" onClick={saveSettings}>
						Save Settings
					</Button>
					<Button
						variant="contained"
						color="secondary"
						onClick={startProgram}
						sx={{ display: "flex", alignItems: "center" }}
						disabled={ !selectedGame || !selectedProgram || loading || (status?.status === "Starting" || status?.status === "Running") }
					>
						{loading || (status && (status.status === "Starting" || status.status === "Running")) ? <CircularProgress size={24} /> : "Start Program"}
					</Button>
				</Box>
			</Box>

			<Box sx={{ display: "flex", width: "100%" }}>
				{/* Game Selection & Program Selection */}
				<Box sx={{ flex: 1, pr: 4 }}>
					{/* Game Selection Dropdown */}
					<FormControl fullWidth sx={{ mt: 2 }}>
						<InputLabel id="game-selection-label">Select Game</InputLabel>
						<Select
							labelId="game-selection-label"
							value={selectedGame}
							onChange={handleGameChange}
							label="Select Game"
						>
							{games.map((game) => (
								<MenuItem key={game} value={game}>
									{game}
								</MenuItem>
							))}
						</Select>
					</FormControl>

					{/* Program Selection List */}
					{selectedGame && programs.length > 0 && (
						<Box sx={{ width: "100%", mt: 3 }}>
							<List>
								{programs.map((program) => (
									<ListItem
										component="button"
										key={program.id}
										onClick={() => handleProgramSelect(program)}
										sx={{
											backgroundColor: theme.palette.background.paper,
											borderRadius: 1,
											border: `1px solid ${theme.palette.divider}`,
											boxShadow: 3,
											'&:hover': {
												backgroundColor: theme.palette.primary.dark,
											},
											padding: "8px 16px",
											marginBottom: 1,
											display: "flex",
											alignItems: "center",
											justifyContent: "center",
										}}
										//selected={selectedProgram?.id === program.id}
									>
										<ListItemText
											primary={program.name}
											sx={{
											  color: theme.palette.text.primary, // Use the primary text color from the theme
											}}
										/>

									</ListItem>
								))}
							</List>
						</Box>
					)}
				</Box>

				{/*Program Settings */}
				<Box sx={{ flex: 1 }}>
					{selectedProgram && (
						<Box sx={{ display: "flex", flexDirection: "column", gap: 2, mt: 2 }}>
							<Box sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
								{Object.keys(selectedProgram.settings).map((key) => (
									<TextField
										color="secondary"
										key={key}
										fullWidth
										label={formatKey(key)}
										value={settings[key] || ""}
										onChange={handleSettingChange(key)}
										sx={{ mb: 2 }}
										type={typeof settings[key] === "number" ? "number" : "text"}
									/>
								))}
							</Box>
						</Box>
					)}
				</Box>
			</Box>
		</Box>
	);
}
