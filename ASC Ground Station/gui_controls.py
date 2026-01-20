import tkinter as tk
from tkinter import ttk

class GroundStationGUI:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("ASC-2680 Ground Station")

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
    
    def load_buttons(self, button_map):
        for btn in button_map.keys():
            var = tk.StringVar(value=f"{btn}: OFF")
            lbl = ttk.Label(self.button_frame, textvariable=var)
            lbl.pack(anchor="w")
            self.button_labels[btn] = var

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
