"use client";

import * as React from "react";
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
	CircularProgress,
} from "@mui/material";
import ExpandMoreIcon from "@mui/icons-material/ExpandMore";
import type {SelectChangeEvent} from "@mui/material";
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

	// Determine if button should be disabled
	const isStatusStartingOrRunning = status && (status.status === "Starting" || status.status === "Running");
	const buttonDisabled = loading || isStatusStartingOrRunning || false;

	// Access window only after the component has mounted
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

				if (selectedGame) {
					setPrograms(data[selectedGame] || []);
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

	// Handle program change
	const handleProgramChange = (program: Program) => {
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
				mt: "64px",
				width: "100%",
				maxWidth: 1200,
				mx: "auto",
				px: 2,
			}}
		>
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
			<StatusDisplay />

			{/* Start Program / Save Settings */}
			<Box sx={{display: "flex", gap: 2, mt: 2}}>
				<Button variant="contained" color="primary" onClick={saveSettings}>
					Save Settings
				</Button>
				<Button
					variant="contained"
					color="secondary"
					onClick={startProgram}
					sx={{display: "flex", alignItems: "center"}}
					disabled={!selectedGame || !selectedProgram || loading || (status && (status.status === "Starting" || status.status === "Running"))}
				>
					{loading || (status && (status.status === "Starting" || status.status === "Running")) ? <CircularProgress size={24}/> : "Start Program"}
				</Button>
			</Box>

			{/* Game Selection Accordion */}
			<Accordion sx={{mt: 2, width: "100%"}}>
				<AccordionSummary
					expandIcon={<ExpandMoreIcon/>}
					aria-controls="game-selection-content"
					id="game-selection-header"
				>
					<Typography>{selectedGame || "Select Game"}</Typography>
				</AccordionSummary>
				<AccordionDetails>
					<List>
						{games.map((game) => (
							<ListItem
								key={game}
								onClick={() => handleGameChange({target: {value: game}} as SelectChangeEvent<string>)}
								sx={{
									cursor: "pointer",
									"&:hover": {
										backgroundColor: theme.palette.action.hover,
									},
								}}
							>
								<ListItemText primary={game}/>
							</ListItem>
						))}
					</List>
				</AccordionDetails>
			</Accordion>

			{/* Program Selection Accordion */}
			{selectedGame && (
				<Accordion sx={{mt: 2, width: "100%"}}>
					<AccordionSummary
						expandIcon={<ExpandMoreIcon/>}
						aria-controls="program-selection-content"
						id="program-selection-header"
					>
						<Typography>{selectedProgram ? selectedProgram.name : "Select Program"}</Typography>
					</AccordionSummary>
					<AccordionDetails>
						<List>
							{programs.map((program) => (
								<ListItem
									key={program.id}
									onClick={() => handleProgramChange(program)}
									sx={{
										cursor: "pointer",
										"&:hover": {
											backgroundColor: theme.palette.action.hover,
										},
									}}
								>
									<ListItemText primary={program.name}/>
								</ListItem>
							))}
						</List>
					</AccordionDetails>
				</Accordion>
			)}

			{/* Program Settings Accordion */}
			{selectedProgram && (
				<Accordion sx={{mt: 2, width: "100%"}}>
					<AccordionSummary
						expandIcon={<ExpandMoreIcon/>}
						aria-controls="program-settings-content"
						id="program-settings-header"
					>
						<Typography>Program Settings for {selectedProgram.name}</Typography>
					</AccordionSummary>
					<AccordionDetails>
						<Box sx={{width: "100%"}}>
							{Object.keys(selectedProgram.settings).map((key) => (
								<TextField
									key={key}
									fullWidth
									label={formatKey(key)}
									value={settings[key] || ""}
									onChange={handleSettingChange(key)}
									sx={{mb: 2}}
									type={typeof settings[key] === "number" ? "number" : "text"}
								/>
							))}
						</Box>
					</AccordionDetails>
				</Accordion>
			)}
		</Box>
	);
}
