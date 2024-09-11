// programs/JSONInterface.ts

export interface Program {
    id: number;
    name: string;
    settings: { [key: string]: any };
}

export interface StatusResponse {
    status: 'Starting' | 'Running' | 'Error' | 'Finished' | 'Pending';
    currentGame?: {
        id: string;
        name: string;
    };
    currentProgram?: {
        id: number;
        name: string;
        settings: { [key: string]: any };
    };
}

export class JSONInterface {
    // Fetch programs and games from the server
    async fetchPrograms(): Promise<{ [key: string]: Program[] }> {
        try {
            const response = await fetch('/api/programs/get-programs');
            if (!response.ok) {
                throw new Error('Failed to fetch programs.');
            }
            return await response.json();
        } catch (error) {
            console.error(error);
            throw error;
        }
    }

    // Fetch the status of the current game/program
    async fetchStatus(): Promise<StatusResponse> {
        try {
            const response = await fetch('/api/programs/get-program-status');
            if (!response.ok) {
                throw new Error('Failed to fetch status');
            }
            return await response.json();
        } catch (error) {
            console.error(error);
            throw error;
        }
    }

    // Save the updated settings for a selected program
    async saveSettings(game: string, programId: number, updatedSettings: { [key: string]: any }): Promise<void> {
        try {
            const response = await fetch('/api/programs/save-settings', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ game, programId, updatedSettings }),
            });

            if (!response.ok) {
                throw new Error('Failed to save settings.');
            }
        } catch (error) {
            console.error(error);
            throw error;
        }
    }
}
