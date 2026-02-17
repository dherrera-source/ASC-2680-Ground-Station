import cv2
from ultralytics import YOLO
import time
from yolo_gui import ControlGUI
import threading

# -----------------------------
# STREAM SETTINGS
# -----------------------------
STREAM_URL = "http://192.168.4.24:8080/stream.mjpeg"

# -----------------------------
# SHARED SETTINGS DICT
# -----------------------------
settings = {
    "model": "yolo26n.pt",
    "conf": 0.35,
    "iou": 0.45,
    "imgsz": 640,
    "hud_enabled": True,
    "track_enabled": True,
    "custom_text": "",
}

# -----------------------------
# TRANSPARENT HUD OVERLAY
# -----------------------------
def draw_hud(frame, fps, num_dets, num_tracks, settings):
    """
    Draws a clean HUD overlay on the YOLO output frame.
    """
    overlay = frame.copy()

    # Background panel
    cv2.rectangle(overlay, (10, 10), (310, 150), (0, 0, 0), -1)
    alpha = 0.35 # 35% opacity

    # HUD text lines
    lines = [
        f"FPS: {fps:.1f}",
        f"Detections: {num_dets}",
        f"Tracks: {num_tracks}",
        f"Model: {settings['model']}",
        f"Conf: {settings['conf']}",
        f"IOU: {settings['iou']}",
        f"Img Size: {settings['imgsz']}",
    ]

    y = 35
    for line in lines:
        cv2.putText(frame, line, (20, y),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6,
                    (0, 255, 0), 2)
        y += 20

    # Custom HUD text
    if settings["custom_text"]:
        cv2.putText(frame, settings["custom_text"], 
                    (20, frame.shape[0] - 20),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, 
                    (0, 255, 255), 2)
    return frame

# -----------------------------
# YOLO SETTINGS
# -----------------------------
MODEL_PATH = "yolo26n.pt"     # model size: n, s, m, l
CONF_THRESHOLD = 0.35         # confidence threshold
IOU_THRESHOLD = 0.45          # NMS IoU threshold
IMG_SIZE = 640                # input resolution
CLASSES = None                # e.g., [0] for person only, None for all

# -----------------------------
# BYTE TRACK SETTINGS
# -----------------------------
TRACK_PERSIST = True          # keep IDs across frames
TRACK_MAX_AGE = 30            # how long to keep lost tracks
TRACK_CONF = 0.1              # minimum confidence for tracking
TRACK_IOU = 0.3               # IoU threshold for association

settings = {
    "model": MODEL_PATH,
    "conf": CONF_THRESHOLD,
    "iou": IOU_THRESHOLD,
    "imgsz": IMG_SIZE,
    "hud_enabled": True,
    "track_enabled": True,
    "custom_text": "",
}

# -----------------------------
# YOLO + BUTE TRACK LOOP (THREAD)
def run_yolo_loop(settings):
    model = YOLO(settings["model"])

    cap = cv2.VideoCapture(STREAM_URL)
    if not cap.isOpened():
        print("Could not open stream")
        return
    
    print("Stream opened successfully. Running YOLO + ByteTrack...")

    prev_time = time.time()

    cap.set(cv2.CAP_PROP_BUFFERSIZE, 1) # reduce latency

    while True:
        ret, frame = cap.read()
        if not ret:
            continue

        # Reduce latency

        for _ in range(5):
            cap.grab()
        ret, frame = cap.retrieve()
        if not ret:
            continue

        frame = cv2.resize(frame, (600, 360))

        # YOLO inference

        results = model.track(
            frame,
            persist=settings["track_enabled"],
            conf=settings["conf"],
            iou=settings["iou"],
            imgsz=settings["imgsz"],
            tracker="bytetrack.yaml"
        )

        # Extract detections

        num_dets = len(results[0].boxes) if results[0].boxes is not None else 0
        num_tracks = len(results[0].boxes.id) if results[0].boxes is not None and results[0].boxes.id is not None else 0

        annotated = results[0].plot()

        # FPS

        now = time.time()
        fps = 1 / max(now - prev_time, 1e-6)
        prev_time = now

        # HUD

        if settings["hud_enabled"]:
            annotated = draw_hud(
                annotated, 
                fps,
                num_dets,
                num_tracks,
                settings
            )
        
        cv2.imshow("YOLO + ByteTrack Output", annotated)

        if cv2.waitKey(1) == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

# -----------------------------
# MAIN
# -----------------------------

if __name__ == "__main__":
    gui = ControlGUI(settings)

    threading.Thread(target=run_yolo_loop, args=(settings,), daemon=True).start()

    gui.root.mainloop()
