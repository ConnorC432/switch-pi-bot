import atexit
from flask import Flask, request, jsonify
from flask_cors import CORS
from programrunner import ProgramRunner
from capturecard import CaptureCard


app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "http://localhost:3000"}})

program_runner = ProgramRunner()

capture_card = None
try:
    capture_card = CaptureCard()
    print("Capture Card initialized")
except ValueError as e:
    print(f"Capture Card {e} will not be initialized")


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
        return capture_card.get_stream()
    else:
        return jsonify({'Error': 'No capture card initialised'}), 503


def cleanup():
    global capture_card
    if capture_card:
        del capture_card


atexit.register(cleanup)


if __name__ == '__main__':
    app.run(debug=True, use_reloader=False)
