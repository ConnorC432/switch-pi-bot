import { NextResponse, NextRequest } from 'next/server';
import fs from 'fs';
import path from 'path';

// Adjust the path if needed
const jsonFilePath = path.join(process.cwd(), '../data/settings.json');

// Define TypeScript interfaces
interface Setting {
    name: string;
    value: any;
}

interface SettingsGroup {
    [key: string]: Setting[];
}

interface UpdatedSettings {
    [group: string]: Setting[];
}

// GET /api/settings - Retrieve settings
export async function GET(request: NextRequest) {
    try {
        // Read existing JSON data
        const jsonData = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8')) as SettingsGroup;

        // Respond with the JSON data
        return NextResponse.json(jsonData);
    } catch (error) {
        console.error('Error retrieving settings:', error);
        return NextResponse.json({ message: 'Internal Server Error', error: error.message }, { status: 500 });
    }
}

// POST /api/settings - Update settings
export async function POST(request: NextRequest) {
    try {
        // Parse request body
        const updatedSettings: UpdatedSettings = await request.json();

        // Log received data for debugging
        console.log('Request received:', updatedSettings);

        // Read existing JSON data
        const jsonData: SettingsGroup = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8'));

        let settingsUpdated = false;

        // Iterate over each group in the updated settings
        for (const [group, settings] of Object.entries(updatedSettings)) {
            if (jsonData[group]) {
                // Iterate over each setting in the group
                for (const setting of settings) {
                    const { name, value } = setting;
                    const settingIndex = jsonData[group].findIndex((s: Setting) => s.name === name);

                    if (settingIndex !== -1) {
                        // Update the setting value
                        jsonData[group][settingIndex].value = value;
                        settingsUpdated = true;
                    } else {
                        console.warn('Setting not found:', name);
                        return NextResponse.json({ message: `Setting ${name} not found.` }, { status: 404 });
                    }
                }
            } else {
                console.warn('Settings group not found:', group);
                return NextResponse.json({ message: `Settings group ${group} not found.` }, { status: 404 });
            }
        }

        if (!settingsUpdated) {
            return NextResponse.json({ message: 'No settings updated.' }, { status: 400 });
        }

        // Write updated data back to JSON file
        fs.writeFileSync(jsonFilePath, JSON.stringify(jsonData, null, 2), 'utf8');
        return NextResponse.json({ message: 'Settings saved successfully.' });
    } catch (error) {
        console.error('Error saving settings:', error);
        return NextResponse.json({ message: 'Internal Server Error', error: error.message }, { status: 500 });
    }
}
