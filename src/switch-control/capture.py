import cv2
import numpy as np
import pytesseract
import time


class Capture:
	def __init__(self):
		"""
		Initializes the CaptureAnalyser with a fixed API URL.
		"""
		self.url = "http://127.0.0.1:8080/stream"
		self.capture = cv2.VideoCapture(self.url)
		if not self.capture.isOpened():
			raise ValueError(f"Failed to connect to URL: {self.url}")
		print(f"Initialized CaptureAnalyser with URL: {self.url}")

	def read_image(self, bounding_box: tuple = None) -> np.ndarray:
		"""
		Fetches the image from the API MJPEG stream and decodes it into a NumPy array.
		Args:
			bounding_box (tuple): Bounding box coordinates (x,y,width,height), default is None.
		Returns:
			np.ndarray: The decoded image as a NumPy array.
		"""
		print(f"Fetching image from {self.url}...")

		# Close and reopen the capture to avoid stale frames
		self.capture.release()
		self.capture = cv2.VideoCapture(self.url)

		success, frame = self.capture.read()
		if not success:
			print("Failed to capture frame from the MJPEG stream.")
			return None

		if bounding_box is not None:
			x, y, w, h = bounding_box
			frame = frame[y:y + h, x:x + w]

		print("Image fetched and decoded successfully.")
		return frame

	def look_for_image(self, template_path: str, threshold: float = 0.8, bounding_box: tuple = None) -> bool:
		"""
		Compares the captured image with a template image to find a match.
		Args:
			template_path (str): Path to the template image file.
			threshold (float): Matching threshold, default is 0.8.
			bounding_box (tuple): Bounding box coordinates (x,y,width,height), default is None.
		Returns:
			bool: True if a match is found, otherwise False.
		"""
		print(f"Looking for image match with template: {template_path}...")
		try:
			template = cv2.imread(template_path, cv2.IMREAD_COLOR)
			if template is None:
				print(f"Error loading template image from {template_path}")
				return False

			captured_image = self.read_image(bounding_box)
			if captured_image is None:
				return False

			gray_image = cv2.cvtColor(captured_image, cv2.COLOR_BGR2GRAY)
			gray_template = cv2.cvtColor(template, cv2.COLOR_BGR2GRAY)

			result = cv2.matchTemplate(gray_image, gray_template, cv2.TM_CCOEFF_NORMED)
			_, max_val, _, _ = cv2.minMaxLoc(result)

			match_found = max_val >= threshold
			print(f"Match value: {max_val}, Threshold: {threshold}, Match found: {match_found}")
			return match_found

		except cv2.error as e:
			print(f"Error during image processing: {e}")
			return False

	def wait_for_image_match(self, template_path, time_value, threshold=0.8, bounding_box: tuple = None):
		"""
		Waits for an image match within a specified time.
		Args:
			template_path (str): Path to the template image file.
			time_value (int): Time to wait in seconds.
			threshold (float): Matching threshold, default is 0.8.
			bounding_box (tuple): Bounding box coordinates (x,y,width,height), default is None.
		Returns:
			bool: True if a match is found within the time, otherwise False.
		"""
		print(f"Waiting for image match with template: {template_path} for up to {time_value} seconds...")
		start_time = time.time()
		while True:
			if self.look_for_image(template_path, threshold, bounding_box):
				print(f"Image match found within {time_value} seconds.")
				return True
			elapsed_time = time.time() - start_time
			if elapsed_time > time_value:
				print(f"No image match found within {time_value} seconds.")
				return False
			time.sleep(1)

	def read_text(self, bounding_box: tuple = None):
		"""
		Extracts text from the captured image using OCR.
		Args:
		    bounding_box (tuple): Bounding box coordinates (x,y,width,height), default is None.
		Returns:
			str: The extracted text.
		"""
		print("Extracting text from the captured image...")
		captured_image = self.read_image(bounding_box)

		if captured_image is None:
			print("No image captured for text extraction.")
			return ""

		gray_image = cv2.cvtColor(captured_image, cv2.COLOR_BGR2GRAY)
		text = pytesseract.image_to_string(gray_image)
		print(f"Extracted text: {text.strip()}")
		return text

	def look_for_text(self, search_text, bounding_box: tuple = None):
		"""
		Checks if the specified text is present in the captured image.
		Args:
			search_text (str): The text to search for.
		    bounding_box (tuple): Bounding box coordinates (x,y,width,height), default is None.
		Returns:
			bool: True if the text is found, otherwise False.
		"""
		print(f"Looking for text: '{search_text}'...")
		recognized_text = self.read_text()
		found = search_text.lower() in recognized_text.lower()
		print(f"Text found: {found}")
		return found

	def wait_for_text(self, search_text, time_value, bounding_box: tuple = None):
		"""
		Waits for the specified text to appear within a specified time.
		Args:
			search_text (str): The text to search for.
			time_value (int): Time to wait in seconds. (None = infinite)
		    bounding_box (tuple): Bounding box coordinates (x,y,width,height), default is None.
		Returns:
			bool: True if the text is found within the time, otherwise False.
		"""
		print(f"Waiting for text: '{search_text}' to appear within {time_value} seconds...")
		start_time = time.time()
		while True:
			if self.look_for_text(search_text):
				print(f"Text '{search_text}' found within {time_value} seconds.")
				return True
			if time_value is not None and (time.time() - start_time) >= time_value:
				print(f"Text '{search_text}' not found within {time_value} seconds.")
				return False
			time.sleep(1)


if __name__ == "__main__":
	analyser = Capture()
