import cv2                                 # OpenCV for video processing
from ultralytics import YOLO               # Import the YOLO class from the ultralytics package

model = YOLO("yolo26n.pt")                 # Load pretrained YOLO model
results = model.track(                     # YOLO mode, model.track or model.predict
                source=0,                  # Use webcam (0 for default, 1 for external)
                classes=0,                 # Only detect class 0 (person)
                tracker="bytetrack.yaml",  # Use ByteTrack for tracking
                show=True,                 # Display results in real-time
                stream=True,                 # Save results to 'runs/detect' directory
                
                
                )

# Print Track IDs
for r in results:
    if r.boxes is not None:
        for box in r.boxes:
            track_id = int(box.id) if box.id is not None else -1
            print("Track ID:", track_id)


# Draw track ID on video
for box in r.boxes:
    track_id = int(box.id) if box.id is not None else -1
    x1, y1, x2, y2 = box.xyxy[0]
    cv2.putText(
        r.orig_img,
        f"ID {track_id}",
        (int(x1), int(y1) - 10),
        cv2.FONT_HERSHEY_SIMPLEX,
        0.6
        (0, 255, 0),
        2
    )

# Exit the program when 'l' key is pressed
while True:
    if cv2.waitKey(1) & 0xFF == ord('l'):
        break
cv2.destroyAllWindows()

# *IF RAM BECOMES AN ISSUE*
# Inference results accumulate in RAM unless 'stream=True' is set, 
# which allows for real-time processing without memory overflow