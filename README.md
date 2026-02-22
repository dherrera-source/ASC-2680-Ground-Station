
- **Drone_ESP** (on‑drone firmware)  
- **Ground Station** (Python operator app)  
- **Ground Station Controller ESP** (the ESP32 that acts as the physical controller interface)  

Here’s an updated, polished **top‑level README.md** that includes all three parts and ties them together into a cohesive system.

You can drop this directly into your repo root as `README.md`.

---

# **README.md — Autonomous Security Drone System**

## **Overview**
This repository contains the full software stack for a modular, scalable **Autonomous Security Drone System**. The system is designed to support manual flight, automated missions, real‑time video analysis, and future multi‑operator access across a facility.

The project is composed of three major components:

- **Drone_ESP** — Embedded firmware running on the drone’s ESP32  
- **Ground Station** — Python‑based operator interface, video pipeline, and mission engine  
- **Ground Station Controller ESP** — ESP32‑based hardware controller interface for manual flight  

Together, these components form the foundation of a fully autonomous patrol and response platform.

---

## **Project Goals**
The system is being developed to support:

- Automated takeoff, navigation, and landing  
- Scheduled patrol routes  
- Real‑time video streaming and object detection  
- Physical controller input via ESP32  
- Safety‑first reactive overrides  
- Multi‑operator access via future backend services  
- Facility‑wide situational awareness  

A core design principle is that **no trained drone operator should be required**. The system handles flight, routing, and safety autonomously.

---

## **Repository Structure**

```
/
├── Drone_ESP/
│   ├── comms/
│   ├── sensors/
│   ├── utils/
│   ├── main.cpp
│   └── README.md
│
├── ground_station/
│   ├── gui/
│   ├── video/
│   ├── control/
│   ├── telemetry/
│   ├── utils/
│   ├── main.py
│   └── README.md
│
├── controller_esp/
│   ├── src/
│   ├── include/
│   ├── main.cpp
│   └── README.md   (future)
│
└── README.md   (this file)
```

Each module is self‑contained and designed for clarity, safety, and future expansion.

---

## **Drone_ESP (On‑Drone Firmware)**
The ESP32 firmware handles:

- WiFi communication (UDP heartbeat + future telemetry)
- Battery monitoring (main + auxiliary)
- GPS scaffolding with safe fallback behavior
- Ultrasonic sensor stubs with validity guards
- Stable main loop with clean logging

The firmware is hardened to run safely even when hardware is disconnected or uninitialized, making it ideal for early development and testing.

See `Drone_ESP/README.md` for details.

---

## **Ground Station (Python Application)**
The ground station provides:

- Unified input pipeline for manual control  
- Real‑time video feed with YOLO + ByteTrack  
- HUD overlay and operator feedback  
- Modular GUI with clean process management  
- Telemetry receiver scaffolding  
- Architecture for future mission engine and autonomous sequences  

This system is the operator‑side backbone for both manual and autonomous flight modes.

See `ground_station/README.md` for details.

---

## **Ground Station Controller ESP**
This ESP32 module acts as the **physical controller interface** for the ground station. It reads hardware inputs (joysticks, switches, buttons) and sends normalized control packets to the ground station over USB or WiFi.

### Current capabilities:
- Reads analog joystick axes  
- Reads digital buttons and switches  
- Normalizes input ranges  
- Sends JSON control packets to the ground station  
- Provides stable, low‑latency manual control  

### Future enhancements:
- Haptic feedback  
- OLED status display  
- Wireless mode  
- Battery monitoring for the controller itself  
- Configurable input mapping  

This module ensures that manual flight remains smooth, responsive, and consistent across hardware setups.

---

## **Autonomy Roadmap**
The project is actively evolving toward:

- Automated mission execution  
- AI‑assisted route selection  
- Reactive interrupt layer (safety + tracking)  
- Facility map visualization  
- Multi‑operator access via backend API  
- Mobile access for security personnel  
- Multi‑drone coordination  

These features will integrate seamlessly with the existing firmware and ground‑station architecture.

---

## **Status**
All three components — Drone_ESP, Ground Station, and Controller ESP — are stable, modular, and ready for the upcoming autonomous demo. The system is designed for long‑term scalability and real‑world deployment in security environments.
