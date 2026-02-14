import tkinter as tk
from tkinter import ttk
import subprocess
import threading
import time
import send_controls
from send_controls import (
    shutdown_socket,
    send_controls_packet,
    send_trim,
    send_trim_set,
    TRIM_STEP,
    reset_all_trims,
    dispatch_button,
    KEY_TO_BUTTON,
)
class TrimControl(tk.Frame):
    def __init__(self, parent, axis, send_trim, send_trim_set, *args, **kwargs):
        super().__init__(parent, *args, **kwargs)
        self.axis = axis
        self.send_trim = send_trim
        self.send_trim_set = send_trim_set

        # Label
        ttk.Label(self, text = f"{axis.capitalize()} Trim").pack()

        #Slider (absolute trim)
        self.slider = ttk.Scale(
            self,
            from_=-0.25,
            to=0.25,
            orient="horizontal",
            command=self.on_slider
        )
        self.slider.set(0.0)
        self.slider.pack(fill="x", padx=5)

        # Buttons (incremental trim)
        btn_frame = tk.Frame(self)
        btn_frame.pack()

        tk.Button(btn_frame, text="<", width = 3,
                  command=lambda: self.adjust(-TRIM_STEP)).pack(side="left")
        tk.Button(btn_frame, text=">", width = 3,
                    command=lambda: self.adjust(+TRIM_STEP)).pack(side="left")

        # Value label

        self.value_label = ttk.Label(self, text="0.00 V")
        self.value_label.pack()
    
    def adjust(self, delta):
        print(f"Trim adjust {self.axis}: {delta}") #debug

        self.send_trim(self.axis, delta)
        new_val = self.slider.get() + delta
        self.slider.set(new_val)
        self.value_label.config(text=f"{new_val:.2f} V")

    def on_slider(self, value):
        print(f"Trim set{self.axis}: {value}")  #debug

        if not hasattr(self, "value_label"):
            return
        
        value = float(value)
        self.send_trim_set(self.axis, value)
        self.value_label.config(text=f"{value:.2f} V")

class GroundStationGUI:
    def __init__(self, root):
        self.root = root

        from send_controls import load_trim_config

        self.trim_values = load_trim_config()

        self.scrcpy_process = None
        self.video_enabled = False
        self.handshake_active = False

        self.bind_keys()

        self.root.title("ASC-2680 Ground Station")

        # --- Create Layout Zones ---
        self.left_frame = tk.Frame(root)
        self.center_frame = tk.Frame(root)
        self.right_frame = tk.Frame(root)

        self.left_frame.grid(row=0, column=0, sticky = "n", padx=10, pady=10)
        self.center_frame.grid(row=0, column=1, sticky="n", padx=10, pady=10)
        self.right_frame.grid(row=0, column=2, sticky="n", padx=10, pady=10)

        self.create_left_widgets()
        self.create_center_widgets()
        self.create_right_widgets()

    def create_left_widgets(self):
        print("CREATE LEFT WIDGETS START")
        # Mode label
        self.mode_var = tk.StringVar(value="Mode: Manual")
        ttk.Label(self.left_frame, textvariable=self.mode_var, font=("Arial, 14")).pack(pady=5)

        #Stick readouts
        self.throttle_var = tk.StringVar(value="Throttle: 0.00")
        self.yaw_var = tk.StringVar(value="Yaw: 0.00")
        self.pitch_var = tk.StringVar(value="Pitch: 0.00")
        self.roll_var = tk.StringVar(value="Roll: 0.00")

        ttk.Label(self.left_frame, textvariable=self.throttle_var).pack()
        ttk.Label(self.left_frame, textvariable=self.yaw_var).pack()
        ttk.Label(self.left_frame, textvariable=self.pitch_var).pack()
        ttk.Label(self.left_frame, textvariable=self.roll_var).pack()

        # --- Controller Connect ---
        self. sync_button = tk.Button(
            self.left_frame,
            text="Controller Connect",
            command=self.run_connect_sequence
        )
        self.sync_button.pack(pady=5)

        # --- Video Feed Toggle Button ---
        self.video_button = tk.Button(
            self.left_frame,
            text="Enable Video Feed",
            command=self.toggle_video_feed
        )
        self.video_button.pack(pady=5)

        # Shutdown Button
        self.shutdown_buttton = tk.Button(
            self.left_frame,
            text="SHUTDOWN PROGRAM",
            command=self.shutdown_program,
            bg="#b33939",
            fg="white"
        )
        self.shutdown_buttton.pack(pady=5)
        print("CREATE WIDGETS END")

    def create_center_widgets(self):
                # Trim Controls
        trim_frame = ttk.LabelFrame(self.center_frame, text = "Trim Controls")
        trim_frame.pack(fill = "x", padx = 10, pady=10)

        self.trim_throttle = TrimControl(trim_frame, "throttle", send_trim, send_trim_set)
        self.trim_throttle.pack(fill="x")
        self.trim_pitch = TrimControl(trim_frame, "pitch", send_trim, send_trim_set)
        self.trim_pitch.pack(fill="x")
        self.trim_yaw = TrimControl(trim_frame, "yaw", send_trim, send_trim_set)
        self.trim_yaw.pack(fill="x")
        self.trim_roll = TrimControl(trim_frame, "roll", send_trim, send_trim_set)
        self.trim_roll.pack(fill="x")

        self.trim_throttle.slider.configure(command=None) # Disable slider callback during setup
        self.trim_pitch.slider.configure(command=None) # Disable slider callback during setup
        self.trim_yaw.slider.configure(command=None) # Disable slider callback during setup
        self.trim_roll.slider.configure(command=None) # Disable slider callback during setup

        self.trim_throttle.slider.set(self.trim_values["throttle"])
        self.trim_pitch.slider.set(self.trim_values["pitch"])
        self.trim_yaw.slider.set(self.trim_values["yaw"])
        self.trim_roll.slider.set(self.trim_values["roll"])

        self.trim_throttle.slider.configure(command=self.trim_throttle.on_slider) #RE-enable callback
        self.trim_pitch.slider.configure(command=self.trim_pitch.on_slider) #RE-enable callback
        self.trim_yaw.slider.configure(command=self.trim_yaw.on_slider) #RE-enable callback
        self.trim_roll.slider.configure(command=self.trim_roll.on_slider) #RE-enable callback

        send_trim_set("throttle", self.trim_values["throttle"])
        send_trim_set("pitch", self.trim_values["pitch"])       
        send_trim_set("yaw", self.trim_values["yaw"])    
        send_trim_set("roll", self.trim_values["roll"])

        # Reset Trims Button
        reset_btn = ttk.Button(
            trim_frame,
            text = "Reset All Trims",
            command=self.reset_all_trims_gui
        )
        reset_btn.pack(pady=5)

    def create_right_widgets(self):
        # Button State
        self.button_frame = ttk.LabelFrame(self.right_frame, text="Buttons")
        self.button_frame.pack(pady=10)
        self.button_labels = {}

        #Create GUI Buttons
        self.control_frame = ttk.LabelFrame(self.right_frame, text="Controls")
        self.control_frame.pack(pady=10)

        ttk.Button(self.control_frame, text="Power",
                   command=lambda: self.on_button_press("power")).pack(fill="x")
        ttk.Button(self.control_frame, text="Takeoff / Land",
                   command=lambda: self.on_button_press("takeoff_land")).pack(fill="x")
        ttk.Button(self.control_frame, text="Fly Speed",
                   command=lambda: self.on_button_press("speed")).pack(fill="x")
        ttk.Button(self.control_frame, text="Stunt Roll",
                   command=lambda: self.on_button_press("stunt")).pack(fill="x")
        
        self.root.bind("<Key>", self.on_key_press)

    def on_button_press(self, name):
        self.last_button_label.config(text=f"Last Button: {name}")
        dispatch_button(name)
    
    def bind_keys(self):
        self.root.bind("<Key>", self.on_key_press)

    def on_key_press(self, event):
        key = event.keysyym.lower()
        if key in KEY_TO_BUTTON:
            dispatch_button(KEY_TO_BUTTON[key])

    def load_buttons(self, button_map):
        for btn in button_map.keys():
            var = tk.StringVar(value=f"{btn}: OFF")
            lbl = ttk.Label(self.button_frame, textvariable=var)
            lbl.pack(anchor="w")
            self.button_labels[btn] = var

    def reset_all_trims_gui(self):
        # Reset stored values + send to ESP32
        reset_all_trims()

        # Update sliders visually

        self.trim_throttle.slider.set(0.0)
        self.trim_pitch.slider.set(0.0)
        self.trim_yaw.slider.set(0.0)
        self.trim_roll.slider.set(0.0)

        #Update labels

        self.trim_throttle.value_label.cnfig(text="0.00 V")
        self.trim_pitch.value_label.config(text="0.00 V")
        self.trim_yaw.value_label.config(text="0.00 V")
        self.trim_roll.value_label.config(text="0.00 V")

    def run_connect_sequence(self):
        self.mode = "manual"
        self.handshake_active = True
        time.sleep(0.01)

        threading.Thread(target=self.connect_sequence, daemon=True).start()

    def connect_sequence(self):
        
        self.handshake_active = True
        self.send_handshake_values(1,1)
        time.sleep(3.0)
        self.send_handshake_values(-1,-1)
        time.sleep(3.0)
        self.send_handshake_values(1,1)
        time.sleep(3.0)

        for _ in range(2):
            self.send_handshake_values(1,-1)
            time.sleep(3.0)
            self.send_handshake_values(-1,1)
            time.sleep(3.0)

        self.send_handshake_values(0,0)
        self.handshake_active = False

    def send_handshake_values(self, throttle, pitch):
        send_controls_packet({
            "throttle": throttle,
            "yaw": 0,
            "pitch": pitch,
            "roll": 0,
            "buttons": {}
        })

    def toggle_video_feed(self):
        if not self.video_enabled:
            try:
                self.scrcpy_process = subprocess.Popen(
                    [r"C:\School\SCRCPY\scrcpy-win64-v3.3.4\scrcpy.exe", 
                     "--max-size", "1080"]
                )
                self.video_button.config(text="Disable Video Feed")
                self.video_enabled = True
            except Exception as e:
                print(f"Failed to start scrcpy: {e}")
        
        else:
            if self.scrcpy_process:
                self.scrcpy_process.terminate()
                self.scrcpy_process = None

            self.video_button.config(text="Enable Video Feed")
            self.video_enabled = False

    def shutdown_program(self):
        import os
        # Kill SCRCPY
        try:
            if hasattr(self, "scrcpy_process") and self.scrcpy_process:
                self.scrcpy_process.terminate()
        except:
            pass
        # Close UDP socket
        try:
            shutdown_socket()
        except:
            pass
        #Destroy GUI window
        try:
            self.root.destroy()
        except:
            pass

        os._exit(0)
    
    def update(self, throttle, yaw, pitch, roll, mode, button_state):
        self.mode_var.set(f"Mode: {mode.upper()}")

        self.throttle_var.set(f"Throttle: {throttle:.2f}")
        self.yaw_var.set(f"Yaw: {yaw:.2f}")
        self.pitch_var.set(f"Pitch: {pitch:.2f}")
        self.roll_var.set(f"Roll: {roll:.2f}")

        for btn, var in self.button_labels.items():
            if btn in button_state:
                var.set(f"{btn}: ON")
            else:
                var.set(f"{btn}: OFF")
    
        self.root.update_idletasks()
        self.root.update()
