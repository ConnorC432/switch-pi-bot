import atexit
import os
import json  # Added for loading JSON files
from flask import Flask, request, jsonify, Response
from flask_cors import CORS
from programrunner import ProgramRunner
from capturecard import CaptureCard

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "http://localhost:3000"}})

program_runner = ProgramRunner()

capture_card = None


# Function to read the Capture Card Device from settings.json
def load_capture_card_device_config(capture_device):
    try:
        with open(capture_device, 'r') as file:
            config = json.load(file)

            # Find the "Capture Card Device" in the "Pi Settings" section
            pi_settings = config.get("Pi Settings", [])
            for setting in pi_settings:
                if setting.get("name") == "Capture Card Device":
                    device_path = setting.get("value")

                    # Extract the number at the end of "/dev/video"
                    if device_path and device_path.startswith("/dev/video"):
                        device_number = device_path.split('/dev/video')[-1]
                        if device_number.isdigit():
                            return int(device_number)
        return None
    except (json.JSONDecodeError, FileNotFoundError) as e:
        print(f"Error loading JSON config: {e}")
        return None

use_capture_card = os.getenv('DISABLE_CAPTURE_CARD', 'True') == 'True'

if not use_capture_card:
    try:
        # Load the capture card device number from settings.json
        json_file_path = os.path.join(os.getcwd(), '../data/settings.json')
        capture_device_number = load_capture_card_device_config(json_file_path)

        if capture_device_number is not None:
            # Initialize the capture card with the extracted video device number
            capture_card = CaptureCard(capture_device_number)
            print(f"Capture Card initialized with /dev/video{capture_device_number}")
        else:
            print("Capture Card device not found or invalid in JSON configuration.")
    except (ValueError, FileNotFoundError) as e:
        print(f"Capture Card error: {e} - will not be initialized")


@app.route('/start-program', methods=['POST'])
def start_program():
    try:
        data = request.get_json()
        game = data.get('game')
        program_id = data.get('programId')

        response, status_code = program_runner.start_program(game, program_id)
        return jsonify(response), status_code

    except Exception as e:
        print(f"Error: {e}")
        return jsonify({'Error': 'Internal Server Error', 'Details': str(e)}), 500


@app.route('/video-stream')
def video_stream():
    if capture_card:
        return Response(capture_card.get_stream(), mimetype='multipart/x-mixed-replace; boundary=frame')
    else:
        return jsonify({'Error': 'No capture card initialised'}), 503


def cleanup():
    global capture_card
    if capture_card:
        del capture_card


atexit.register(cleanup)

if __name__ == '__main__':
    app.run(debug=False, use_reloader=False)
