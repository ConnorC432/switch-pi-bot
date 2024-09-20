import os
import sys
import json
import importlib.util


# TODO match root_dir to new directory location
base_dir = os.path.dirname(os.path.abspath(__file__))
if os.path.basename(os.getcwd()) == 'switch-control':
	root_dir = os.path.abspath(os.path.join(base_dir, "..", "switch-control"))
else:
	root_dir = os.path.abspath(os.path.join(base_dir, "../../../switch-control"))


def program(settings):
	# Import Capture Analyser class
	spec = importlib.util.spec_from_file_location("CaptureAnalyser", (os.path.join(root_dir, "capture_analyser.py")))
	capture_analyser = importlib.util.module_from_spec(spec)
	spec.loader.exec_module(capture_analyser)

	# JSON Settings to Variables [Setting name from JSON, Default Value]
	wait_time = int(settings.get("WaitTime", "1"))
	image_name = os.path.join(root_dir, "assets", settings.get("Image"))

	# Put Script Code Here

	analyser = capture_analyser.CaptureAnalyser()
	if analyser.wait_for_image_match(image_name, wait_time, 0.35):
		return True
	else:
		return False


def parse_args(args):
	settings = {}
	for arg in args:
		if "=" in arg:
			key, value = arg.split("=", 1)
			settings[key] = value
	return settings


def main():
	# Retrieve settings
	settings = parse_args(sys.argv[1:])
	for key, value in settings.items():
		globals()[key] = value

	print("Program Settings:")
	for key in settings:
		print(f"{key} =  {globals().get(key)}")

	# Run program and determine handle failure/success
	status = program(settings)
	status_message = "Finished" if status else "Error"

	status_file_path = os.path.abspath(os.path.join(root_dir, "../data/status.json"))
	if os.path.exists(status_file_path):
		print(f"Modifying JSON file: {status_file_path}")
		with open(status_file_path) as status_file:
			status_data = json.load(status_file)
	else:
		status_data = {}

	status_data['status'] = status_message

	# Write to JSON
	try:
		print(f"writing status data to {status_file_path}")
		with open(status_file_path, "w") as status_file:
			json.dump(status_data, status_file, indent=2)
	except Exception as e:
		print(e)

	sys.exit(0 if status else 1)


if __name__ == "__main__":
	main()
