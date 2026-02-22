
# **README.md — Drone_ESP Firmware**

## **Overview**
The `Drone_ESP` module contains the embedded firmware for the drone‑mounted ESP32.  
This firmware is responsible for sensor acquisition, WiFi communication, telemetry generation, and future autonomous behaviors. The current implementation focuses on building a clean, modular foundation that is safe, stable, and easy to extend.

This directory was previously empty; this is the first functional firmware drop.

---

## **Current Features**

### **WiFiLink Module**
- Connects ESP32 to facility WiFi in STA mode  
- Initializes UDP socket for telemetry transmission  
- Sends periodic heartbeat packets  
- Clean connection state tracking  
- Structured for future reconnect logic and packet routing  

### **Battery Monitoring**
Two independent battery sensing modules:

#### **BatteryMain**
- Reads main LiPo voltage through ADC  
- Low‑pass filtered output  
- Sanity checks for floating pins or invalid readings  

#### **BatteryAux**
- Identical architecture for auxiliary battery  
- Independent ADC pin and filtering  
- Returns structured `BatteryAuxData`  

Both modules are hardened against noise, invalid readings, and wiring issues.

### **GPS Module (Scaffolding)**
- Safe default values when no fix is present  
- Guards against invalid or missing data  
- Structured for future NMEA parsing or UBX integration  

### **Ultrasonic Sensors (Stubbed)**
- Left and right sensors return `valid=0` when disconnected  
- Prevents crashes or undefined behavior  
- Ready for HC‑SR04 or similar integration  

### **Main Loop Integration**
- Heartbeat timer  
- Periodic sensor updates  
- Clean logging output  
- No‑crash behavior even with missing hardware  
- Modular structure for future telemetry packets  

---

## **Architecture**
The firmware is organized into small, focused modules:

```
Drone_ESP/
│
├── comms/
│   ├── wifi_link.cpp/.h
│   └── telemetry.cpp/.h   (future)
│
├── sensors/
│   ├── battery_main.cpp/.h
│   ├── battery_aux.cpp/.h
│   ├── gps.cpp/.h
│   └── ultrasonic.cpp/.h
│
├── utils/
│   └── logger.cpp/.h
│
└── main.cpp
```

Each module is self‑contained and designed for safe operation even when hardware is missing or uninitialized.

---

## **Runtime Behavior**
When powered on, the firmware:

1. Connects to WiFi  
2. Starts heartbeat timer  
3. Updates all sensors at fixed intervals  
4. Logs system state to serial  
5. Sends heartbeat packets over UDP  
6. Maintains stable operation even with missing sensors  

This provides a reliable baseline for future telemetry and autonomous flight logic.

---

## **Future Work**
Planned enhancements include:

- Full telemetry packet generation (JSON or binary)  
- WiFi reconnect logic  
- GPS parsing and fix validation  
- Ultrasonic distance measurement  
- IMU integration  
- Command/Control packet handling  
- Autonomous safety routines  
- Integration with ground station mission engine  

---

## **Status**
This firmware is stable, builds cleanly, and runs without crashes.  
It establishes the foundation for all future drone‑side intelligence and communication.

