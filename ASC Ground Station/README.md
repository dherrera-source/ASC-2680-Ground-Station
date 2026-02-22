
# **README.md — Ground Station**

## **Overview**
The Ground Station is the operator‑side application responsible for controlling the drone, displaying video feeds, running object detection, and eventually executing autonomous missions. It is built in Python with a modular architecture designed for reliability, clarity, and future scalability.

This system is the primary interface for manual control, mission execution, and real‑time monitoring.

---

## **Current Features**

### **Unified Input Pipeline**
- Centralized control path for all manual inputs  
- Ensures consistent behavior across keyboard, GUI buttons, and future controllers  
- Clean separation between input, processing, and output  
- Supports safe overrides and future autonomous sequences  

### **Video Pipeline**
- Stable integration with YOLO + ByteTrack  
- Real‑time object detection  
- HUD overlay for operator feedback  
- Auto‑positioning overlay window for seamless video alignment  
- Modular design for future multi‑camera or multi‑drone support  

### **GUI System**
- Python‑based interface with modular windows  
- Video Controls window with clean shutdown logic  
- Mode indicators and operator feedback  
- Structured for future mission control and facility map integration  

### **Process Management**
- Reliable launch and shutdown of YOLO, ByteTrack, and video processes  
- Environment‑specific Python interpreter detection  
- Clean handling of subprocesses to avoid zombie processes  
- Deterministic behavior across different machines  

### **Telemetry Integration (Scaffolding)**
- Ready to receive UDP telemetry from Drone_ESP  
- Structured parsing pipeline  
- Hooks for battery, GPS, ultrasonic, and system health data  

---

## **Architecture**
The ground station is organized into modular components:

```
ground_station/
│
├── gui/
│   ├── main_window.py
│   ├── video_controls.py
│   └── overlay.py
│
├── video/
│   ├── yolo_runner.py
│   ├── bytetrack_runner.py
│   └── stream_manager.py
│
├── control/
│   ├── input_pipeline.py
│   ├── keybinds.py
│   └── dispatcher.py
│
├── telemetry/
│   ├── receiver.py
│   └── parser.py
│
├── utils/
│   ├── logger.py
│   └── config.py
│
└── main.py
```

Each module is designed to be self‑contained, testable, and easy to extend.

---

## **Runtime Behavior**
When launched, the ground station:

1. Initializes the GUI  
2. Starts the video pipeline  
3. Loads YOLO + ByteTrack  
4. Activates the unified input pipeline  
5. Connects to telemetry (if available)  
6. Displays real‑time detections and operator feedback  

The system is designed to remain stable even if the drone is offline or telemetry is missing.

---

## **Autonomy Support (In Progress)**
The ground station is being expanded to support:

- Automated flight sequences  
- Mission Control mode  
- Route selection and scoring  
- Reactive interrupt layer  
- Facility map visualization  
- Multi‑operator access via backend API  

These features will integrate with the Drone_ESP firmware and future backend services.

---

## **Future Work**
Planned enhancements include:

- Full mission engine with maneuver sequencing  
- AI‑assisted route selection  
- Web dashboard for remote operators  
- Mobile access for security staff  
- Multi‑drone coordination  
- Alarm routing and event logging  
- Backend API integration (FastAPI)  

---

## **Status**
The ground station is stable, modular, and ready for the upcoming autonomous demo.  
It forms the operator‑side backbone of the full autonomous security drone system.
