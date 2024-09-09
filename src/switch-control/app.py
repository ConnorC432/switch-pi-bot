from flask import Flask, request, jsonify
from flask_cors import CORS
import json
import os
import subprocess

app = Flask(__name__)

CORS(app, resources={r"/*": {"origins": "http://localhost:3000"}})

# Define base directory for the scripts (adjust as necessary)
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
SCRIPTS_DIR = os.path.join(BASE_DIR, 'programs')  # All game/program scripts will be in the 'scripts' directory

STATUS_FILE_PATH = os.path.join(BASE_DIR, '../data/status.json')
PROGRAMS_FILE_PATH = os.path.join(BASE_DIR, '../data/programs.json')


@app.route('/start-program', methods=['POST'])
def start_program():
    try:
        # Parse the incoming request data
        data = request.get_json()
        game = data.get('game')
        program_id = data.get('programId')

        # Read the status file
        with open(STATUS_FILE_PATH, 'r') as status_file:
            status_data = json.load(status_file)

        # Read the programs file
        with open(PROGRAMS_FILE_PATH, 'r') as programs_file:
            programs_data = json.load(programs_file)

        # Find the selected game
        game_data = programs_data.get(game)
        if not game_data:
            return jsonify({'error': 'Game not found'}), 404

        # Find the selected program
        program = next((p for p in game_data if p['id'] == program_id), None)
        if not program:
            return jsonify({'error': 'Program not found'}), 404

        # Update the status data
        status_data['currentGame'] = {
            'id': game,
            'name': game  # Assuming the game name in statusData is the same as the key
        }
        status_data['currentProgram'] = {
            'id': program_id,
            'name': program['name'],
            'settings': program.get('settings', {})
        }
        status_data['status'] = 'Starting'

        # Write the updated status to the status file
        with open(STATUS_FILE_PATH, 'w') as status_file:
            json.dump(status_data, status_file, indent=2)

        # Build the path to the Python script based on the game and program name
        script_path = os.path.join(SCRIPTS_DIR, game, f"{program['name']}.py")

        # Check if the script exists
        if not os.path.isfile(script_path):
            status_data['status'] = 'Error'
            with open(STATUS_FILE_PATH, 'w') as status_file:
                json.dump(status_data, status_file, indent=2)
            return jsonify({'error': f"Script not found at {script_path}"}), 404

        # Prepare arguments
        settings_args = []
        settings = program.get('settings', {})
        for key, value in settings.items():
            settings_args.append(f'{key}={value}')

        # Run the script using subprocess
        try:
            print(f"Attempting to start: {script_path}")
            # This runs the script in a new process
            process = subprocess.Popen(
                ["python", script_path] + settings_args,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                cwd=BASE_DIR
            )

            # Optionally capture stdout/stderr or return it in the response
            stdout, stderr = process.communicate()

            return_code = process.returncode
            if return_code != 0:
                status_data['status'] = 'Error'
                with open(STATUS_FILE_PATH, 'w') as status_file:
                    json.dump(status_data, status_file, indent=2)
                print(f"Script execution failed with return code {return_code}")
                print(f"stdout: {stdout.decode()}")
                print(f"stderr: {stderr.decode()}")
                return jsonify({
                    'error': 'Script execution failed',
                    'stdout': stdout.decode(),
                    'stderr': stderr.decode()
                }), 500

            status_data['status'] = 'Finished'
            with open(STATUS_FILE_PATH, 'w') as status_file:
                json.dump(status_data, status_file, indent=2)

            print(f"Script executed successfully")
            return jsonify({'status': 'Finished', 'stdout': stdout.decode(), 'stderr': stderr.decode()}), 200

        except Exception as e:
            status_data['status'] = 'Error'
            with open(STATUS_FILE_PATH, 'w') as status_file:
                json.dump(status_data, status_file, indent=2)

            print(f"Failed to execute script: {e}")
            return jsonify({'error': f"Failed to execute script: {str(e)}"}), 500

    except Exception as e:
        status_data['status'] = 'Error'
        with open(STATUS_FILE_PATH, 'w') as status_file:
            json.dump(status_data, status_file, indent=2)
        print(f"Failed to start program: {e}")
        return jsonify({'error': 'Failed to start program', 'details': str(e)}), 500


if __name__ == '__main__':
    app.run(debug=True)
