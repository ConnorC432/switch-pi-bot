import cv2
import os

def test_video_device(device_id):
    cap = cv2.VideoCapture(device_id)
    if cap.isOpened():
        cap.release()
        return True
    return False

def find_valid_video_devices():
    valid_devices = []
    # Check devices from /dev/video0 to /dev/videoN (adjust N as needed)
    for device_id in range(99):  # Change 10 if you need to check more devices
        if test_video_device(device_id):
            valid_devices.append(f"/dev/video{device_id}")
    return valid_devices

if __name__ == "__main__":
    valid_devices = find_valid_video_devices()
    if valid_devices:
        print("Valid capture cards found:")
        for device in valid_devices:
            print(device)
    else:
        print("No valid capture cards found.")
