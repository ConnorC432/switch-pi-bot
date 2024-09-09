import { NextResponse } from 'next/server';
import fs from 'fs';
import path from 'path';

// Define file paths
const statusFilePath = path.join(process.cwd(), '../data/status.json');
const programsFilePath = path.join(process.cwd(), '../data/programs.json');

export async function POST(request: Request) {
    try {
        const { game, programId } = await request.json();

        // Read the existing status file
        const statusFile = fs.readFileSync(statusFilePath, 'utf-8');
        const statusData = JSON.parse(statusFile);

        // Read the programs file
        const programsFile = fs.readFileSync(programsFilePath, 'utf-8');
        const programsData = JSON.parse(programsFile);

        // Find the selected game
        const gameData = programsData[game];
        if (!gameData) {
            return NextResponse.json({ error: 'Game not found' }, { status: 404 });
        }

        // Find the selected program
        const program = gameData.find((p: any) => p.id === programId);
        if (!program) {
            return NextResponse.json({ error: 'Program not found' }, { status: 404 });
        }

        // Update the status data
        statusData.currentGame = {
            id: game,
            name: game // Assuming the game name in statusData is the same as the key
        };
        statusData.currentProgram = {
            id: programId,
            name: program.name,
            settings: program.settings
        };
        statusData.status = 'Starting';

        // Write the updated status to the file
        fs.writeFileSync(statusFilePath, JSON.stringify(statusData, null, 2));

        return NextResponse.json({ status: 'Starting' });
    } catch (error) {
        console.error('Failed to start program:', error);
        return NextResponse.json({ error: 'Failed to start program', details: error.message }, { status: 500 });
    }
}
