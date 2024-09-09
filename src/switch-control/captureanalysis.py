from io import BytesIO
import time
import cv2
import numpy as np
import pytesseract
import requests


class CaptureAnalyser:
    def __init__(self, url: str = "http://127.0.0.1:5000/video-stream"):
        """
        Initializes the CaptureAnalyser with a given API URL.
        """
        self.url = url

    def read_image(self) -> np.ndarray:
        """
        Fetches the image from the API and decodes it into a NumPy array.
        Returns:
            np.ndarray: The decoded image as a NumPy array.
        """
        try:
            response = requests.get(self.url, stream=True)
            response.raise_for_status()

            image_bytes = BytesIO(response.content)
            image = np.asarray(bytearray(image_bytes.read()), dtype="uint8")
            image = cv2.imdecode(image, cv2.IMREAD_COLOR)

            if image is None:
                print(f"Error decoding image from {self.url}")
            return image

        except requests.RequestException as e:
            print(f"Error fetching image from {self.url}: {e}")
            return None
        except cv2.error as e:
            print(f"Error decoding image: {e}")
            return None

    def look_for_image(self, template_path: str, threshold: float = 0.8) -> bool:
        """
        Compares the captured image with a template image to find a match.
        Args:
            template_path (str): Path to the template image file.
            threshold (float): Matching threshold, default is 0.8.
        Returns:
            bool: True if a match is found, otherwise False.
        """
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

            return max_val >= threshold

        except cv2.error as e:
            print(f"Error during image processing: {e}")

    def wait_for_image_match(self, template_path, time_value, threshold=0.8):
        """
        Waits for an image match within a specified time.
        Args:
            template_path (str): Path to the template image file.
            time_value (int): Time to wait in seconds.
            threshold (float): Matching threshold, default is 0.8.
        Returns:
            bool: True if a match is found within the time, otherwise False.
        """
        start_time = time.time()
        while True:
            if self.look_for_image(template_path, threshold):
                return True
            elapsed_time = time.time() - start_time
            if elapsed_time > time_value:
                return False
            time.sleep(1)

    def read_text(self):
        """
        Extracts text from the captured image using OCR.
        Returns:
            str: The extracted text.
        """
        captured_image = self.read_image()

        if captured_image is None:
            return ""

        gray_image = cv2.cvtColor(captured_image, cv2.COLOR_BGR2GRAY)
        text = pytesseract.image_to_string(gray_image)
        return text

    def look_for_text(self, search_text):
        """
        Checks if the specified text is present in the captured image.
        Args:
            search_text (str): The text to search for.
        Returns:
            bool: True if the text is found, otherwise False.
        """
        recognized_text = self.read_text()
        return search_text.lower() in recognized_text.lower()

    def wait_for_text(self, search_text, time_value):
        """
        Waits for the specified text to appear within a specified time.
        Args:
            search_text (str): The text to search for.
            time_value (int): Time to wait in seconds.
        Returns:
            bool: True if the text is found within the time, otherwise False.
        """
        start_time = time.time()
        while True:
            if self.look_for_text(search_text):
                return True
            elapsed_time = time.time() - start_time
            if elapsed_time >= time_value:
                return False
            time.sleep(1)


if __name__ == "__main__":
    CaptureAnalyser()

    """ 
    Example: Wait for an image match
    template_path = 'path/to/template.jpg'
    timeout = 30
    if analyzer.wait_for_image_match(template_path, timeout):
        print(f"Image match found within {timeout} seconds.")
    else:
        print(f"No image match found within {timeout} seconds.")

    # Example: Wait for specific text
    search_text = 'Example Text'
    timeout = 30
    if analyzer.wait_for_text(search_text, timeout):
        print(f"Text '{search_text}' found within {timeout} seconds.")
    else:
        print(f"Text '{search_text}' not found within {timeout} seconds.")
    """
