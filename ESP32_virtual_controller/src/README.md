
# **README.md — Ground Station Controller ESP**

## **Overview**
The Ground Station Controller ESP is the firmware that powers the **physical controller interface** used by the operator. It runs on an ESP32 and acts as a hardware bridge between joysticks, switches, buttons, and the Ground Station application.

This module ensures smooth, low‑latency manual control and provides a consistent input experience across different hardware setups. It is designed to be simple, robust, and easily extendable as new controller features are added.

---

## **Current Features**

### **Analog Input Handling**
- Reads joystick axes using ESP32 ADC  
- Normalizes raw ADC values into a clean 0.0–1.0 or −1.0–1.0 range  
- Applies optional deadzones and smoothing  
- Hardened against floating pins or invalid readings  

### **Digital Input Handling**
- Reads buttons, toggles, and switches  
- Debouncing logic for stable state transitions  
- Structured mapping for future custom controller layouts  

### **Control Packet Output**
- Packages all controller inputs into a structured JSON control packet  
- Sends packets to the Ground Station over USB serial or WiFi (depending on configuration)  
- Low‑latency, consistent update rate  
- Designed for future binary packet support  

### **Modular Architecture**
- Clean separation between input reading, normalization, and packet generation  
- Easy to add new inputs (e.g., sliders, dials, mode switches)  
- Ready for haptic feedback or OLED display integration  

---

## **Architecture**

```
controller_esp/
│
├── src/
│   ├── input_reader.cpp/.h
│   ├── packet_builder.cpp/.h
│   ├── controller.cpp/.h
│   └── main.cpp
│
├── include/
│   └── config.h
│
└── README.md
```

Each module is focused and testable:

- **input_reader** — Reads and normalizes hardware inputs  
- **packet_builder** — Formats data into JSON or binary packets  
- **controller** — High‑level logic and update loop  
- **main.cpp** — Initialization and runtime loop  

---

## **Runtime Behavior**
When powered on, the controller:

1. Initializes ADC and GPIO pins  
2. Reads joystick axes and button states  
3. Normalizes and debounces inputs  
4. Builds a control packet  
5. Sends the packet to the Ground Station at a fixed rate  
6. Repeats with low latency and stable timing  

This ensures responsive, predictable manual control.

---

## **Future Enhancements**
Planned improvements include:

- Wireless mode (WiFi or BLE)  
- OLED status display (battery, mode, connection)  
- Haptic feedback for alerts or mission events  
- Configurable input mapping  
- Battery monitoring for the controller itself  
- Support for additional hardware modules (sliders, dials, gimbals)  

---

## **Status**
The Ground Station Controller ESP provides a stable and extensible foundation for manual drone control. It integrates cleanly with the Ground Station application and will continue to evolve as the autonomy and mission systems expand.
