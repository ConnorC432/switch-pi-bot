import { NextResponse, NextRequest } from 'next/server';
import fs from 'fs';
import path from 'path';

// Define the path to the status and programs JSON files
const statusFilePath = path.join(process.cwd(), '../data/status.json');
const programsFilePath = path.join(process.cwd(), '../data/programs.json');

// Define the structure of status data
interface StatusData {
    status: 'Starting' | 'Running' | 'Error' | 'Finished';
    currentGame?: {
        name: string;
    };
    currentProgram?: {
        name: string;
        id: number;
        settings: { [key: string]: string };
    };
}

// Define the structure of a template
interface Template {
    id: number;
    name: string;
    settings: { [key: string]: string };
}

// Define the structure of the JSON data
interface ProgramData {
    templates: Template[];
}

export async function POST(request: NextRequest) {
    try {
        // Parse request body
        const { game, programId } = await request.json();

        // Log received data for debugging
        console.log('Request received:', { game, programId });

        // Read the existing status file
        const statusFile = fs.readFileSync(statusFilePath, 'utf-8');
        const statusData: StatusData = JSON.parse(statusFile);

        // Read the programs file
        const programsFile = fs.readFileSync(programsFilePath, 'utf-8');
        const programsData: ProgramData = JSON.parse(programsFile);

        // Find the selected program
        const selectedProgram = programsData.templates.find(template => template.id === programId);

        if (selectedProgram) {
            // Here you would add your logic to start the program
            // For example, you might update the status file to reflect the current program

            // Update status data
            const updatedStatus: StatusData = {
                ...statusData,
                status: 'Starting',
                currentGame: { name: game },
                currentProgram: selectedProgram,
            };

            // Write updated status back to JSON file
            fs.writeFileSync(statusFilePath, JSON.stringify(updatedStatus, null, 2), 'utf-8');

            return NextResponse.json({ message: 'Program started successfully.' });
        } else {
            console.error('Program not found:', programId);
            return NextResponse.json({ message: 'Program not found.' }, { status: 404 });
        }
    } catch (error) {
        console.error('Error starting program:', error);
        return NextResponse.json({ message: 'Internal Server Error', error: error.message }, { status: 500 });
    }
}
