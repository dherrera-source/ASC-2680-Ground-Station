import tkinter as tk
from tkinter import ttk
import subprocess

class GroundStationGUI:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("ASC-2680 Ground Station")

        # Track scrcpy state

        self.scrcpy_process = None

        self.video_enabled = False


        # Mode label

        self.mode_var = tk.StringVar(value="Mode: Manual")
        ttk.Label(self.root, textvariable=self.mode_var, font=("Arial, 14")).pack(pady=5)

        #Stick readouts

        self.throttle_var = tk.StringVar()
        self.yaw_var = tk.StringVar()
        self.pitch_var = tk.StringVar()
        self.roll_var = tk.StringVar()

        ttk.Label(self.root, textvariable=self.throttle_var).pack()
        ttk.Label(self.root, textvariable=self.yaw_var).pack()
        ttk.Label(self.root, textvariable=self.pitch_var).pack()
        ttk.Label(self.root, textvariable=self.roll_var).pack()

        # Button State

        self.button_frame = ttk.LabelFrame(self.root, text="Buttons")
        self.button_frame.pack(pady=10)
    
        self.button_labels = {}

        # --- Video Feed Toggle Button ---

        self.video_button = tk.Button(
            self.root,
            text="Enable Video Feed",
            command=self.toggle_video_feed
        )
        self.video_button.pack(pady=5)
        #------------------------------
    
    def load_buttons(self, button_map):
        for btn in button_map.keys():
            var = tk.StringVar(value=f"{btn}: OFF")
            lbl = ttk.Label(self.button_frame, textvariable=var)
            lbl.pack(anchor="w")
            self.button_labels[btn] = var

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

            self.video_button.cofig(text="Enable Video Feed")
            self.video_enabled = False
    #--------------------------------------------------------------------
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
