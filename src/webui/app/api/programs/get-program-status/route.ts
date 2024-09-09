import { NextResponse } from 'next/server';
import fs from 'fs';
import path from 'path';

const statusFilePath = path.join(process.cwd(), '../data/status.json');

export async function GET() {
    try {
        const statusFile = fs.readFileSync(statusFilePath, 'utf-8');
        const statusData = JSON.parse(statusFile);
        return NextResponse.json(statusData);
    } catch (error) {
        console.error('Failed to fetch status:', error);
        return NextResponse.json({ error: 'Failed to fetch status' }, { status: 500 });
    }
}
