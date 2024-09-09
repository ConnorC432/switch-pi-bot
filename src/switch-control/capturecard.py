import cv2
from flask import Response


class CaptureCard:
    def __init__(self, device_index=0):
        self.device_index = device_index
        self.cap = cv2.VideoCapture(device_index)

        if not self.cap.isOpened():
            raise ValueError(f"Unable to open video device at index {device_index}")

        # 1920 x 1080p
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)
        # 30 fps
        self.cap.set(cv2.CAP_PROP_FPS, 30)

        print(f"Capture Card initialized with device index {device_index}")

    def get_stream(self):
        while self.cap.isOpened():
            ret, frame = self.cap.read()
            if not ret:
                break

        _, buffer = cv2.imencode('.jpg', frame)
        frame_bytes = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

    def __del__(self):
        if self.cap.isOpened():
            self.cap.release()
        print("Capture Card released")
