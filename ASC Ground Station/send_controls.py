import socket
import json
import time
import sys
import select
import tkinter as tk
import msvcrt
import ctypes
import subprocess
import threading
import os

TRIM_FILE = "trim_config.json"

def launch_scrcpy():
    subprocess.Popen([r"C:\School\SCRCPY", "--max-size", "800"])

with open("button_map.json", "r") as f:
    BUTTON_MAP = json.load(f)

from pynput import mouse

def load_trim_config():
    if not os.path.exists(TRIM_FILE):
        return{"throttle":0.0, "yaw":0.0, "pitch":0.0, "roll":0.0}
    
    try:
        with open(TRIM_FILE, "r") as f:
            data = f.read().strip()
            if not data:

                return{"throttle":0.0, "yaw":0.0, "pitch":0.0, "roll":0.0}
            return json.loads(data)
    except json.JSONDecodeError:
        # FIle exists but is corrupted
        return{"throttle":0.0, "yaw":0.0, "pitch":0.0, "roll":0.0}
    
    
def save_trim_config(trim_dict):
    with open(TRIM_FILE, "w") as f:
        json.dump(trim_dict, f, indent = 4)
# --- Mouse Globals ---

mouse_dx = 0
mouse_dy = 0

# --- Mouse Handler ---

def on_move(x, y):
    global mouse_dx, mouse_dy
    mouse_dx = x
    mouse_dy = y

# --- Mouse Listener ---

mouse_listener = mouse.Listener(on_move=on_move)
mouse_listener.start()

#--- Configuration ---

ESP32_IP = "192.168.0.69" # Replace with your ESP32's IP address
ESP32_PORT = 14550 #MUST MATCH UDP.BEGIN() ON ESP32

PACKET_RATE = 20        #PACKETS PER SECOND
DELAY = 1.00 / PACKET_RATE

#--- UDP SETUP ---

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setblocking(False)

_last_debug = 0

# --- Global State ---

button_state = {} # { "button_name": {"pressed_at": timestamp} }

AUTO_STATE = {
    "start_time": None,
    "phase": 0
}

TRIM_STEP = 0.1 # Volts per click

def send_trim(axis, delta):
    packet = {
        "type": "trim",
        "axis": axis,
        "delta": delta
    }
    sock.sendto(json.dumps(packet).encode(), (ESP32_IP, ESP32_PORT))
    print("Sending:", packet) #debug

    trims = load_trim_config()
    trims[axis] += delta
    save_trim_config(trims)

def send_trim_set(axis, value):
    packet = {
        "type": "trim_set",
        "axis": axis,
        "value": value
    }
    sock.sendto(json.dumps(packet).encode(), (ESP32_IP, ESP32_PORT))
    print("Sending:", packet) #debug

    trims = load_trim_config()
    trims[axis] = value
    save_trim_config(trims)

# --- Button Library ---

KEY_TO_BUTTON = {
    "t": "power",
    "l": "takeoff_land",
    "s": "speed",
    "x": "stunt",
    "v": "photo", # short press photo, long press video

    "1": "trim_throttle_up",
    "2": "trim_throttle_down",
    "3": "trim_yaw_left",
    "4": "trim_yaw_right",
    "5": "trim_pitch_up",
    "6": "trim_pitch_down",
    "7": "trim_roll_left",
    "8": "trim_roll_right",
}

#-----------------------------------------------------------
# --- INPUT HANDLING LAYER ---
#------------------------------------------------------------
# Keyboard handler
def handle_manual_buttons():
    if msvcrt.kbhit():
        key = msvcrt.getwch().lower()
        if key in KEY_TO_BUTTON:
            dispatch_button(KEY_TO_BUTTON[key])

# Long-press logic (shared)
def handle_long_press_logic():
    #Hold 'photo' for 1 second to start video
    if is_held("photo", 1.0):
        dispatch_button("video_start")
        release("photo")

# --- CENTRAL DISPATCHER ---

speed_down = False
stund_down = False

def dispatch_button(name):
    global speed_down, stunt_down

    print(f"[DEBUG] Dispatching button: {name}")

    # Track combo state
    if name == "speed":
        speed_down = True
    if name == "stunt":
        stunt_down = True

    # Emergency stop combo
    if speed_down and stund_down:
        print("[DEBUG] Emergency stop triggered!")
        press("emergency_stop")
        speed_down = False
        stunt_down = False
        return
    
    # Normal button press
    press(name)

# --- Helpers ---

def clamp(v, lo=-1.0, hi=1.0):
    return max(lo, min(hi, v))

def build_packet(throttle, yaw, pitch, roll, buttons=None):
    if buttons is None:
        buttons = {}
    
    packet = {
        "throttle": clamp(throttle),
        "yaw": clamp(yaw),
        "pitch": clamp(pitch),
        "roll": clamp(roll),
        "buttons": buttons,
        "t": time.time()
    }

    return json.dumps(packet).encode()

def send_controls(throttle, yaw, pitch, roll, buttons=None):
    global _last_debug

    if buttons is None:
        buttons = {name: 1 for name in button_state.keys()}

    payload = {
        "throttle": float(throttle),
        "yaw": float(yaw),
        "pitch": float(pitch),
        "roll": float(roll),
        "buttons": buttons,
        "heartbeat": time.time()
    }

    data = json.dumps(payload).encode("utf-8")
    sock.sendto(data, (ESP32_IP, ESP32_PORT))

    if time.time() - _last_debug > 0.2:
        print("\n === PYTHON SENDING PACKET ===")
        print(f"To: {ESP32_IP}:{ESP32_PORT}")
        print("Payload:", json.dumps(payload))
        _last_debug = time.time()

def send_controls_packet(packet):

    print(f"[DEBUG] Packet to ESP32: {packet}")

    if packet.get("type") == "button":
        sock.sendto(json.dumps(packet).encode(), (ESP32_IP, ESP32_PORT))
        return
    
    throttle = packet.get("throttle", 0)
    yaw = packet.get("yaw", 0)
    pitch = packet.get("pitch", 0)
    roll = packet.get("roll", 0)
    buttons = packet.get("buttons", {})

    send_controls(throttle, yaw, pitch, roll, buttons)

def press(button_name):
    button_state[button_name] = {"pressed_at": time.time()}
    print(f"[DEBUG] press() called for: {button_name}")
    packet = {"type": "button", "name": button_name}
    send_controls_packet(packet)

def release(button_name):
    if button_name in button_state:
        del button_state[button_name]

def is_held(button_name, duration):
        if button_name not in button_state:
            return False
        return (time.time() - button_state[button_name]["pressed_at"]) >= duration

def clear_buttons():
    button_state.clear()

def reset_all_trims():
    trims = {
        "throttle": 0.0,
        "yaw": 0.0,
        "pitch": 0.0,
        "roll": 0.0
    }

    save_trim_config(trims)

    # Push zeros to ESP32

    for axis in trims:
        send_trim_set(axis, 0.0)
# --- Mode Management ---

mode = "manual"  # or "auto"

# --- Manual Input (PLaceholder) ---

def get_manual_inputs():
    global mouse_dx, mouse_dy

    throttle = 0.0
    yaw = 0.0
    pitch = 0.0
    roll = 0.0

    # Windows key state checker
    def key_down(vk):
        return ctypes.windll.user32.GetAsyncKeyState(vk) & 0x8000

    # W / S throttle
    if key_down(0x57):   # W
        throttle = 1.0
    elif key_down(0x53): # S
        throttle = -1.0

    # A / D roll
    if key_down(0x41):   # A
        roll = -1.0
    elif key_down(0x44): # D
        roll = 1.0

    # Q / E yaw
    if key_down(0x51):   # Q
        yaw = -1.0
    elif key_down(0x45): # E
        yaw = 1.0

    #Arrow Pitch
    if key_down(0x26):   # Up Arrow
        pitch = 1.0
    elif key_down(0x28): # Down Arrow
        pitch = -1.0
    # Mouse pitch
    #pitch = -mouse_dy * 0.01

    #mouse_dx = 0
    #mouse_dy = 0

    print("inputs:", throttle, yaw, pitch, roll)
    print("KEY RAW:", ctypes.windll.user32.GetAsyncKeyState(0x57))
    #print("MOUSE RAW:", mouse_dy)

    return throttle, yaw, pitch, roll

# --- Autonomous Input (PLaceholder) ---

def auto_flight_engine():
    now = time.time()

    # Initialize state

    if AUTO_STATE["start_time"] is None:
        AUTO_STATE["start_time"] = now
    
    elapsed = now - AUTO_STATE["start_time"]

    # --- Phase 0: Trigger auto takeoff ---

    if AUTO_STATE["phase"] == 0:
        press("auto_start")
        AUTO_STATE["phase"] = 1
        return 0.5, 0.0, 0.0, 0.0  # Moderate throttle for takeoff
    
    # --- Phase 1: Fly forward for 2 seconds ---
    if AUTO_STATE["phase"] == 1:
        if elapsed < 2.0:
            return 0.5, 0.0, 0.3, 0.0  # Fly forward (THROTTLE, YAW, PITCH, ROLL)
        
        AUTO_STATE["phase"] = 2   

    # --- Phase 2: Trigger auto land ---

    if AUTO_STATE["phase"] == 2:
        press("auto_land")
        AUTO_STATE["phase"] = 3
        return 0.3 , 0.0, 0.0, 0.0  # Cut throttle for landing
    
    # --- Phase 3: Idle ---

    return 0.0, 0.0, 0.0, 0.0

# --- Mode Switching ---

def check_for_mode_switch(current_mode):
    if msvcrt.kbhit():
        key = msvcrt.getwch().lower()
        if key == "m":
            print("Switched to MANUAL mode")
            return "manual"
        elif key == "n":
            print("Switched to AUTO mode")
            return "auto"
    return current_mode   

# --- Shutdown Handler ---
def shutdown_socket():
    try:
        sock.close()
    except:
        pass    

#--- Main Loop ---

def main(gui):
    global mode

    
    print("Ground station running ...")
    print(f"Sending to {ESP32_IP}:{ESP32_PORT}")
    print("Press 'm' for manual mode, 'n' for auto mode")

    while True:
        #Check for mode switch

        mode = check_for_mode_switch(mode)

        # Generate controls based on mode

        if gui.handshake_active:
            continue

            pass
        
        else:
            if mode =="manual":
                handle_manual_buttons()
                throttle, yaw, pitch, roll = get_manual_inputs()
        
            else:
                throttle, yaw, pitch, roll = auto_flight_engine()

            # --- Long-press evaluation (shared) ---
            handle_long_press_logic()

        # Send to ESP32

        print("Sending:", {name:1 for name in button_state.keys()})
        send_controls(throttle, yaw, pitch, roll)  

        gui.update(throttle, yaw, pitch, roll, mode, button_state)

        time.sleep(DELAY)

if __name__ == "__main__":
    from gui_controls import GroundStationGUI
    import tkinter as tk

    root = tk.Tk()
    gui = GroundStationGUI(root)
    gui.load_buttons(BUTTON_MAP)

    control_thread = threading.Thread(target=main, args=(gui,), daemon=True)
    control_thread.start()

    root.mainloop()
    