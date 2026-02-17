import subprocess
import numpy as np
import cv2                                 # OpenCV for video processing
from ultralytics import YOLO               # Import the YOLO class from the ultralytics package

WIDTH = 1280
HEIGHT = 720

model = YOLO("yolo26n.pt")                 # Load pretrained YOLO model

adb_path = r"C:School\SCRCPY\scrcpy-win64-v2.4\adb.exe"

adb_proc = subprocess.Popen(
    [
        adb_path,
        "exec-out",
        "screenrecord",
        "--size", f"{WIDTH}x{HEIGHT}",
        "--bit-rate", "8m",
        "--output-formate=h264",
        "-"
    ],
    stdout=subprocess.PIPE,
    stderr=subprocess.DEVNULL
)

# FFmpeg decodes H.264 - > raw BGR frames

ffmpeg_proc = subprocess.Popen(
    [
        "ffmpeg",
        "-i", "pipe:0",
        "-f", "rawvideo",
        "-pix_fmt", "bgr24",
        "pipe:1"
    ],
    stdin=adb_proc.stdout,
    stdout=subprocess.PIPE,
    stderr=subprocess.DEVNULL
)

frame_size = WIDTH * HEIGHT * 3

while True:
    raw = ffmpeg_proc.stdout.read(frame_size)
    if not raw:
        break

    frame = np.frombuffer(raw, dtype=np.uint8).reshape((HEIGHT, WIDTH, 3))

    # YOLO inference

    results = model.track(frame, persist=True)

    # Draw

    annotated = results[0].plot()

    cv2.imshow("YOLO + ByteTrack", annotated)
    if cv2.waitKey(1) == ord("l"):
        break

adb_proc.kill()
ffmpeg_proc.kill()
cv2.destroyAllWindows()
