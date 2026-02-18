import subprocess
import numpy as np
import cv2

WIDTH = 720
HEIGHT = 1280
FRAME_SIZE = WIDTH * HEIGHT * 3

adb_folder = "C:\School\SCRCPY\scrcpy-win64-v2.4"
adb_path = adb_folder + r"\adb.exe"

# Start ADB screenrecord
adb_proc = subprocess.Popen(
    [
        adb_path,
        "exec-out",
        "screenrecord",
        "--size", f"{WIDTH}x{HEIGHT}",
        "--bit-rate", "8m",
        "--output-format=h264",
        "-"
    ],
    cwd=adb_folder,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE
)

# FFmpeg decode
ffmpeg_proc = subprocess.Popen(
    [
        "ffmpeg",
        "-loglevel", "error",
        "-f", "h264",
        "-i", "pipe:0",
        "-f", "rawvideo",
        "-pix_fmt", "bgr24",
        "pipe:1"
    ],
    stdin=adb_proc.stdout,
    stdout=subprocess.PIPE,
    stderr=subprocess.DEVNULL
)

while True:
    raw = ffmpeg_proc.stdout.read(FRAME_SIZE)
    if not raw:
        print("No data received")
        break

    frame = np.frombuffer(raw, dtype=np.uint8).reshape((HEIGHT, WIDTH, 3))
    cv2.imshow("Phone Stream Test", frame)

    if cv2.waitKey(1) == ord('q'):
        break

adb_proc.kill()
ffmpeg_proc.kill()
cv2.destroyAllWindows()