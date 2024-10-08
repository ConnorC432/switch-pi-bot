import cv2
import numpy as np
import pytesseract
import time

class CaptureAnalyser:
    def __init__(self):
        """
        Initializes the CaptureAnalyser with a fixed API URL.
        """
        self.url = "http://127.0.0.1:5000/video-stream"
        self.capture = cv2.VideoCapture(self.url)
        if not self.capture.isOpened():
            raise ValueError(f"Failed to connect to URL: {self.url}")
        print(f"Initialized CaptureAnalyser with URL: {self.url}")

    def read_image(self) -> np.ndarray:
        """
        Fetches the image from the API MJPEG stream and decodes it into a NumPy array.
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
        print("Image fetched and decoded successfully.")
        return frame

    def look_for_image(self, template_path: str, threshold: float = 0.8) -> bool:
        """
        Compares the captured image with a template image to find a match.
        Args:
            template_path (str): Path to the template image file.
            threshold (float): Matching threshold, default is 0.8.
        Returns:
            bool: True if a match is found, otherwise False.
        """
        print(f"Looking for image match with template: {template_path}...")
        try:
            template = cv2.imread(template_path, cv2.IMREAD_COLOR)
            if template is None:
                print(f"Error loading template image from {template_path}")
                return False

            captured_image = self.read_image()
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

    def wait_for_image_match(self, template_path, time_value, threshold=0.35):
        """
        Waits for an image match within a specified time.
        Args:
            template_path (str): Path to the template image file.
            time_value (int): Time to wait in seconds.
            threshold (float): Matching threshold, default is 0.35.
        Returns:
            bool: True if a match is found within the time, otherwise False.
        """
        print(f"Waiting for image match with template: {template_path} for up to {time_value} seconds...")
        start_time = time.time()
        while True:
            if self.look_for_image(template_path, threshold):
                print(f"Image match found within {time_value} seconds.")
                return True
            elapsed_time = time.time() - start_time
            if elapsed_time > time_value:
                print(f"No image match found within {time_value} seconds.")
                return False
            time.sleep(1)

    def read_text(self):
        """
        Extracts text from the captured image using OCR.
        Returns:
            str: The extracted text.
        """
        print("Extracting text from the captured image...")
        captured_image = self.read_image()

        if captured_image is None:
            print("No image captured for text extraction.")
            return ""

        gray_image = cv2.cvtColor(captured_image, cv2.COLOR_BGR2GRAY)
        text = pytesseract.image_to_string(gray_image)
        print(f"Extracted text: {text.strip()}")
        return text

    def look_for_text(self, search_text):
        """
        Checks if the specified text is present in the captured image.
        Args:
            search_text (str): The text to search for.
        Returns:
            bool: True if the text is found, otherwise False.
        """
        print(f"Looking for text: '{search_text}'...")
        recognized_text = self.read_text()
        found = search_text.lower() in recognized_text.lower()
        print(f"Text found: {found}")
        return found

    def wait_for_text(self, search_text, time_value):
        """
        Waits for the specified text to appear within a specified time.
        Args:
            search_text (str): The text to search for.
            time_value (int): Time to wait in seconds.
        Returns:
            bool: True if the text is found within the time, otherwise False.
        """
        print(f"Waiting for text: '{search_text}' to appear within {time_value} seconds...")
        start_time = time.time()
        while True:
            if self.look_for_text(search_text):
                print(f"Text '{search_text}' found within {time_value} seconds.")
                return True
            elapsed_time = time.time() - start_time
            if elapsed_time >= time_value:
                print(f"Text '{search_text}' not found within {time_value} seconds.")
                return False
            time.sleep(1)

if __name__ == "__main__":
    # Initialize with a fixed URL
    analyser = CaptureAnalyser()  # No need to pass URL

    # Example usage
    template_path = 'path/to/template.jpg'  # Replace with actual template path
    timeout = 3  # Example timeout
    if analyser.wait_for_image_match(template_path, timeout):
        print(f"Image match found within {timeout} seconds.")
    else:
        print(f"No image match found within {timeout} seconds.")

    # Example usage for text search
    search_text = 'DefaultText'
    timeout = 3  # Example timeout
    if analyser.wait_for_text(search_text, timeout):
        print(f"Text '{search_text}' found within {timeout} seconds.")
    else:
        print(f"Text '{search_text}' not found within {timeout} seconds.")
