import os
import sys
import json
import time
import asyncio


base_dir = os.path.dirname(os.path.abspath(__file__))
if os.path.basename(os.getcwd()) == 'switch-control':
	root_dir = os.path.abspath(os.path.join(base_dir, "..", "switch-control"))
else:
	root_dir = os.path.abspath(os.path.join(base_dir, "../../../switch-control"))

sys.path.append(root_dir)

# Script Variables
assets = os.path.join(root_dir, "assets", "BDSP", "egghatcher")
text_bounds = (398, 884, 1141, 163)
stat_bounds = (1345, 213, 161, 510)
stat_value_bounds = [
	(1506, 211, 414, 57),
	(1506, 268, 414, 57),
	(1506, 325, 414, 57),
	(1506, 382, 414, 57),
	(1506, 439, 414, 57),
	(1506, 496, 414, 57),
	(1506, 553, 414, 57),
	(1506, 610, 414, 57)
]
gender_bounds = (1400, 21, 445, 59)
shiny_bounds = (1851, 157, 50, 51)
egg_bounds = [
	(192, 258, 163, 135),
	(192, 405, 163, 135),
	(192, 552, 163, 135),
	(192, 699, 163, 135),
	(192, 846, 163, 135)
]


async def check_hatch():
	CAPTURE.wait_for_text("Oh?", 1800, text_bounds)

async def toggle_bike():
	await GAMEPAD.press_button(0.1, "Plus")
	await GAMEPAD.press_dpad(0.1, "S")

async def position_player():
	await GAMEPAD.move_left_stick(5, "SW")
	for _ in range(8):
		await GAMEPAD.press_dpad(0.1, "E")
		await asyncio.sleep(0.2)
	await toggle_bike()
	await GAMEPAD.press_dpad(5, "N")

async def hatch_spin(stop):
	while not stop.is_set():
		await GAMEPAD.move_left_stick(0.1, "N")
		await GAMEPAD.move_left_stick(0.1, "NE")
		await GAMEPAD.move_left_stick(0.1, "E")
		await GAMEPAD.move_left_stick(0.1, "SE")
		await GAMEPAD.move_left_stick(0.1, "S")
		await GAMEPAD.move_left_stick(0.1, "SW")
		await GAMEPAD.move_left_stick(0.1, "W")
		await GAMEPAD.move_left_stick(0.1, "NW")

async def collect_egg():
	await GAMEPAD.move_left_stick(5, "NW")
	await toggle_bike()
	for _ in range(4):
		await GAMEPAD.press_dpad(0.1, "E")
	for _ in range(18):
		await GAMEPAD.press_dpad(0.1, "S")
	for _ in range(4):
		await GAMEPAD.press_dpad(0.1, "W")
	await GAMEPAD.press_dpad(5, "N")

	await GAMEPAD.press_button(0.1, "A")
	if CAPTURE.wait_for_text("Ah, it's you! We were taking care of your Pokémon, and my goodness, were we surprised!", 10):
		for _ in range(9):
			await GAMEPAD.press_button(0.1, "A")
			await asyncio.sleep(0.2)
			egg_counter += 1
			return True
	else:
		for _ in range(3):
			await GAMEPAD.press_button(0.1, "A")
			await asyncio.sleep(0.2)
			return False

	await position_player()

def check_stats(mon_stats):
	if CAPTURE.look_for_image(os.path.join(assets, "stats.jpg"), 0.8, stat_bounds):
		# Check IVs
		for i, stat in enumerate(stat_value_bounds):
			mon_stats[i] = CAPTURE.read_text(stat)

		# Check Gender
		if CAPTURE.look_for_image(os.path.join(assets, "male.jpg"), 0.8, gender_bounds):
			mon_stats[8] = "Male"
		elif CAPTURE.look_for_image(os.path.join(assets, "female.jpg"), 0.8, gender_bounds):
			mon_stats[8] = "Female"
		else:
			print("Gender can't be determined")
			return False

		# Check Shiny
		if CAPTURE.look_for_image(os.path.join(assets, "shiny.jpg"), 0.8, stat_bounds):
			mon_stats[9] = "Yes"
		else:
			mon_stats[9] = "None"

	else:
		print("View Stats not on correct selection")
		return False

	return mon_stats

def compare_stats(mon_stats, desired_stats):
	if len(mon_stats) != len(desired_stats):
		print("Error Processing Stats")
		return False

	for mon_value, desired_value in zip(mon_stats, desired_stats):
		if mon_value is not None and mon_value != desired_value:
			print("Mon doesn't meet desired stats")
			return False

	return True

async def hatch():
	stop = asyncio.Event()

	while True:
		if egg_counter < 5:
			stop.set()
			stop = asyncio.Event()

			hatch_task = asyncio.create_task(hatch_spin(stop))
			try:
				await asyncio.wait_for(hatch_task, timeout=20)
			except asyncio.TimeoutError:
				hatch_task.cancel()
				await hatch_task
			await collect_egg()

		else:
			stop.clear()
			hatch_task = asyncio.create_task(hatch_spin(stop))

			while egg_counter >= 5:
				await asyncio.sleep(1)

			stop.set()
			await hatch_task


async def program(settings):
	from capture import Capture
	from gamepad import Gamepad
	# JSON Settings to Variables [Setting name from JSON, Default Value]
	desired_stats = [
		settings.get("HP", "None"),
		settings.get("Attack", "None"),
		settings.get("Defense", "None"),
		settings.get("Sp. Atk", "None"),
		settings.get("Sp. Def", "None"),
		settings.get("Speed", "None"),
		settings.get("Nature", "None"),
		settings.get("Ability", "None"),
		settings.get("Gender", "None"),
		settings.get("Shiny", "None"),
	]

	CAPTURE = Capture()
	GAMEPAD = Gamepad()

	egg_counter = 0
	hatched_counter = 0


	# Count eggs already in party
	await GAMEPAD.press_button(0.1, "X")
	await GAMEPAD.press_button(0.1, "A")
	for egg in egg_bounds:
		if CAPTURE.wait_for_text("Egg", 10, egg):
			egg_counter += 1
	for i in range(2):
		await GAMEPAD.press_button(0.1, "B")


	position_player()

	hatch_check = asyncio.create_task(check_hatch())
	walking = asyncio.create_task(hatch())
	# Walk and wait for eggs to hatch
	while True:
		if hatch_check.done():
			print("Egg Hatched")
			walking.cancel()
			await walking

			for _ in range(3):
				GAMEPAD.press_button(0.1, "A")
				await asyncio.sleep(0.2)

			hatched_counter += 1
			hatch_check = asyncio.create_task(check_hatch())
			if hatched_counter >= 5:
				await GAMEPAD.press_button(0.1, "X")
				await GAMEPAD.press_button(0.1, "A")
				await GAMEPAD.press_button(0.1, "R")
				await GAMEPAD.press_dpad(0.1, "W")
				await GAMEPAD.press_dpad(0.1, "S")

				for _ in range(5):
					# Determine Mon Stats
					mon_stats = check_stats([])
					# Keep or Release Mon
					if compare_stats(mon_stats, desired_stats):
						return True
					else:
						await GAMEPAD.press_button(0.1, "A")
						await asyncio.sleep(0.25)
						await GAMEPAD.press_dpad(0.1, "N")
						await GAMEPAD.press_dpad(0.1, "N")
						await GAMEPAD.press_dpad(0.1, "A")
						await asyncio.sleep(0.25)
						await GAMEPAD.press_dpad(0.1, "N")
						await GAMEPAD.press_button(0.1, "A")
						await asyncio.sleep(0.25)
						await GAMEPAD.press_button(0.1, "A")

				hatched_counter = 0

			walking = asyncio.create_task(hatch())


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
