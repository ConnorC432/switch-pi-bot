import { NextResponse } from "next/server";
import fs from 'fs';
import path from 'path';

const jsonFilePath = path.join(process.cwd(), '../data/programs.json');

export async function GET() {
    try {
        console.log('Reading file:', jsonFilePath);
        const jsonData = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8'));
        return NextResponse.json(jsonData);
    } catch (error) {
        console.log('Error reading file:', error);
        return NextResponse.json({message: 'Failed to load file', error: error.message}, {status: 404});
    }
}