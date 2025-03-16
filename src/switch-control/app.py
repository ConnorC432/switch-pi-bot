from flask import Flask, request, jsonify
from flask_cors import CORS
from programrunner import ProgramRunner


app = Flask(__name__)
CORS(app, origins="*")
program_runner = ProgramRunner()


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
		return jsonify({'Error': 'Internal Server Error'}), 500

@app.route('/test', methods=['POST'])
def test():
	return jsonify({'Success': True}), 200


if __name__ == '__main__':
	app.run(debug=False, use_reloader=False)
