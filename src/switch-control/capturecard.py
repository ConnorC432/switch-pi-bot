import cv2
import time


class CaptureCard:
    def __init__(self, device_index=0):
        self.device_index = device_index
        self.cap = cv2.VideoCapture(device_index)
        self.frame_rate = 15

        if not self.cap.isOpened():
            raise ValueError(f"Unable to open video device at index {device_index}")

        # 1920 x 1080p
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)
        # FPS
        self.cap.set(cv2.CAP_PROP_FPS, self.frame_rate)

        print(f"Capture Card initialized with device index {device_index}")

    def get_stream(self):
        frame_delay = 1 / self.frame_rate

        if not self.cap.isOpened():
            return "Camera not available", 503

        ret, frame = self.cap.read()
        if not ret:
            return "Failed to capture frame", 500

        _, buffer = cv2.imencode('.jpg', frame)
        frame_bytes = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

        time.sleep(frame_delay)

    def __del__(self):
        if self.cap.isOpened():
            self.cap.release()
        print("Capture Card released")
