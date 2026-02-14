import cv2                                 # OpenCV for video processing
from ultralytics import YOLO               # Import the YOLO class from the ultralytics package

model = YOLO("yolo26n.pt")                 # Load pretrained YOLO model
results = model.track(                     # YOLO mode, model.track or model.predict
                source=0,                  # Use webcam (0 for default, 1 for external)
                classes=0,                 # Only detect class 0 (person)
                tracker="bytetrack.yaml",  # Use ByteTrack for tracking
                show=True,                 # Display results in real-time
                save=True,                 # Save results to 'runs/detect' directory
                )

# Exit the program when 'q' key is pressed
while True:
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
cv2.destroyAllWindows()

# *IF RAM BECOMES AN ISSUE*
# Inference results accumulate in RAM unless 'stream=True' is set, which allows for real-time processing without memory overflow