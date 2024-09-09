import cv2


class CaptureCard:
    def __init__(self, device_index=1):
        self.device_index = device_index
        self.cap = cv2.VideoCapture(device_index)

        if not self.cap.isOpened():
            raise ValueError(f"Unable to open video device at index {device_index}")

        print(f"Capture Card initialized with device index {device_index}")

    def get_stream(self):
        if not self.cap.isOpened():
            return "Camera not available", 503

        ret, frame = self.cap.read()
        if not ret:
            return "Failed to capture frame", 500

        _, buffer = cv2.imencode('.jpg', frame)
        return buffer.tobytes(), 200

    def __del__(self):
        if self.cap.isOpened():
            self.cap.release()
        print("Capture Card released")
