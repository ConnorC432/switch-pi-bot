import json
import os
import subprocess

class ProgramRunner:
    def __init__(self):
        # Define base directory for the scripts (adjust as necessary)
        self.base_dir = os.path.dirname(os.path.abspath(__file__))
        self.scripts_dir = os.path.join(self.base_dir, 'programs')

        self.status_file_path = os.path.abspath(os.path.join(self.base_dir, '..', 'data', 'status.json'))
        self.programs_file_path = os.path.abspath(os.path.join(self.base_dir, '..', 'data', 'programs.json'))

        print(f"Initialized ProgramRunner with base_dir={self.base_dir}, scripts_dir={self.scripts_dir}, "
              f"status_file_path={self.status_file_path}, programs_file_path={self.programs_file_path}")

    @staticmethod
    def _read_file(file_path):
        print(f"Reading file: {file_path}")
        with open(file_path, 'r') as file:
            return json.load(file)

    @staticmethod
    def _write_file(file_path, data):
        print(f"Writing file: {file_path}")
        with open(file_path, 'w') as file:
            json.dump(data, file, indent=2)

    def _update_status(self, status_message):
        print(f"Updating status to: {status_message}")
        status_data = self._read_file(self.status_file_path)
        status_data['status'] = status_message
        self._write_file(self.status_file_path, status_data)

    def _get_script_path(self, game, program_name):
        script_path = os.path.join(self.scripts_dir, game, f"{program_name}.py")
        print(f"Script path resolved to: {script_path}")
        return script_path

    @staticmethod
    def _run_script(script_path, settings_args):
        print(f"Running script: {script_path} with arguments: {settings_args}")

        process = subprocess.Popen(
            ["python", script_path] + settings_args,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            cwd=(os.path.dirname(script_path))
        )
        stdout, stderr = process.communicate()
        stdout_decoded = stdout.decode()
        stderr_decoded = stderr.decode()

        print(f"Process returned code: {process.returncode}")
        print(f"Process stdout: {stdout_decoded}")
        print(f"Process stderr: {stderr_decoded}")

        return process.returncode, stdout_decoded, stderr_decoded

    def start_program(self, game, program_id):
        try:
            status_data = self._read_file(self.status_file_path)
            programs_data = self._read_file(self.programs_file_path)

            if status_data.get('status') == 'Running':
                return {'error': 'A program is already running.'}, 400

            print(f"Starting program with game={game} and program_id={program_id}")

            game_data = programs_data.get(game)
            if not game_data:
                error_message = f"Game not found: {game}"
                print(error_message)
                return {'error': error_message}, 404

            program = next((p for p in game_data if p['id'] == program_id), None)
            if not program:
                error_message = f"Program not found: {program_id}"
                print(error_message)
                return {'error': error_message}, 404

            print(f"Found program: {program}")

            status_data['currentGame'] = {'id': game, 'name': game}
            status_data['currentProgram'] = {
                'id': program_id,
                'name': program['name'],
                'settings': program.get('settings', {})
            }
            self._update_status('Starting')

            script_path = self._get_script_path(game, program['name'])
            if not os.path.isfile(script_path):
                error_message = f"Script not found at: {script_path}"
                print(error_message)
                self._update_status('Error')
                return {'error': error_message}, 404

            settings_args = [f'{key}={value}' for key, value in program.get('settings', {}).items()]
            print(f"Settings arguments: {settings_args}")
            self._update_status('Running')

            return_code, stdout, stderr = self._run_script(script_path, settings_args)

            if return_code != 0:
                error_message = f"Script execution failed with return code: {return_code}"
                print(error_message)
                self._update_status('Error')
                return {
                    'error': error_message,
                    'stdout': stdout,
                    'stderr': stderr
                }, 500

            print(f"Script executed successfully")
            self._update_status('Finished')
            return {'status': 'Finished', 'stdout': stdout, 'stderr': stderr}, return_code

        except Exception as e:
            error_message = f"Failed to start program: {str(e)}"
            print(error_message)
            self._update_status('Error')
            return {'error': error_message}, 500
