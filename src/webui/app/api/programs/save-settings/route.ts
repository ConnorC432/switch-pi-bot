import { NextResponse, NextRequest } from 'next/server';
import fs from 'fs';
import path from 'path';

// Adjust the path if needed
const jsonFilePath = path.join(process.cwd(), '../data/programs.json');

export async function POST(request: NextRequest) {
    try {
        // Parse request body
        const { game, programId, updatedSettings } = await request.json();

        // Log received data for debugging
        console.log('Request received:', { game, programId, updatedSettings });

        // Read existing JSON data
        const jsonData = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8'));

        // Check if the game exists in the JSON
        if (jsonData[game]) {
            // Find the program by id in the game array
            const programIndex = jsonData[game].findIndex((program: any) => program.id === programId);

            if (programIndex !== -1) {
                // Update the settings of the found program
                jsonData[game][programIndex].settings = updatedSettings;

                // Write updated data back to JSON file
                fs.writeFileSync(jsonFilePath, JSON.stringify(jsonData, null, 2), 'utf8');
                return NextResponse.json({ message: 'Program settings saved successfully.' });
            } else {
                console.error('Program not found:', programId);
                return NextResponse.json({ message: 'Program not found.' }, { status: 404 });
            }
        } else {
            console.error('Game not found:', game);
            return NextResponse.json({ message: 'Game not found.' }, { status: 404 });
        }
    } catch (error) {
        console.error('Error saving settings:', error);
        return NextResponse.json({ message: 'Internal Server Error', error: error.message }, { status: 500 });
    }
}