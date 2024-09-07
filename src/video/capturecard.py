import cv2
import os


def find_capture_cards():
    capture_cards = []

    for device in os.listdir('/dev'):
        if device.startswith('video'):
            #Test device
            try:
                test_capture = cv2.VideoCapture('/dev/' + device)

                if not test_capture.isOpened():
                    print(f"Skipping capture card /dev/{device}")
                    test_capture.release()
                    continue

                capture_cards.append(f"/dev/{device}")
                test_capture.release()

            except Exception as e:
                print(f"Unable to open video device /dev/{device}: {e}")
                continue
    return capture_cards


class VideoCapture:

    def __init__(self, device):
        self.device = device
        self.cap = cv2.VideoCapture(device)

        if not self.cap.isOpened():
            raise OSError(f"Unable to open video device {self.device}")

        def is_open(self):
            return self.cap.isOpened()

        def read_frames(self):
            if not self.cap.isOpened():
                raise OSError(f"Unable to open video device {self.device}")

            ret, frame = self.cap.read()
            if not ret:
                raise OSError(f"Unable to read frame from video device {self.device}")

            return frame

        def release(self):
            if self.cap:
                self.cap.release()

        def __del__(self):
            self.release()