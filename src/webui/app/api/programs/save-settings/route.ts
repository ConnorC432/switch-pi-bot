import { NextResponse, NextRequest } from 'next/server';
import fs from 'fs';
import path from 'path';

// Define the path to the programs JSON file
const jsonFilePath = path.join(process.cwd(), '../data/programs.json');

// Define the structure of a setting
interface Setting {
    [key: string]: string;
}

// Define the structure of a template
interface Template {
    id: number;
    name: string;
    settings: Setting;
}

// Define the structure of the JSON data
interface ProgramData {
    templates: Template[];
}

export async function POST(request: NextRequest) {
    try {
        // Parse request body
        const { programId, updatedSettings } = await request.json();

        // Log received data for debugging
        console.log('Request received:', { programId, updatedSettings });

        // Read existing JSON data
        const jsonData: ProgramData = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8'));

        // Find the program by id in the templates array
        const templateIndex = jsonData.templates.findIndex((template) => template.id === programId);

        if (templateIndex !== -1) {
            // Update the settings of the found template
            jsonData.templates[templateIndex].settings = updatedSettings;

            // Write updated data back to JSON file
            fs.writeFileSync(jsonFilePath, JSON.stringify(jsonData, null, 2), 'utf8');
            return NextResponse.json({ message: 'Program settings saved successfully.' });
        } else {
            console.error('Program not found:', programId);
            return NextResponse.json({ message: 'Program not found.' }, { status: 404 });
        }
    } catch (error) {
        console.error('Error saving settings:', error);
        return NextResponse.json({ message: 'Internal Server Error', error: error.message }, { status: 500 });
    }
}
