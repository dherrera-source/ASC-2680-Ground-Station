import tkinter as tk
from tkinter import ttk
import threading

class ControlGUI:
    def __init__(self, settings):
        self.settings = settings

        self.root = tk.Tk()
        self.root.title("YOLO Control Panel")

        # Confidence slider
        tk.Label(self.root, text="Confidence").pack()
        self.conf_slider = tk.Scale(self.root, from_=0.1, to=0.9, resolution=0.01,
                                    orient="horizontal",
                                    command=self.update_conf)
        self.conf_slider.set(settings["conf"])
        self.conf_slider.pack(fill="x")

        # IOU slider
        tk.Label(self.root, text="IOU Threshold").pack()
        self.iou_slider = tk.Scale(self.root, from_=0.1, to=0.9, resolution=0.01,
                                   orient="horizontal",
                                   command=self.update_iou)
        self.iou_slider.set(settings["iou"])
        self.iou_slider.pack(fill="x")

        # Image size dropdown
        tk.Label(self.root, text="Image Size").pack()
        self.imgsz_var = tk.StringVar(value=str(settings["imgsz"]))
        imgsz_menu = ttk.Combobox(self.root, textvariable=self.imgsz_var,
                                  values=["320", "480", "640", "720"])
        imgsz_menu.bind("<<ComboboxSelected>>", self.update_imgsz)
        imgsz_menu.pack(fill="x")

        # HUD toggle
        self.hud_var = tk.BooleanVar(value=True)
        tk.Checkbutton(self.root, text="Show HUD",
                       variable=self.hud_var,
                       command=self.update_hud).pack()

        # Tracking toggle
        self.track_var = tk.BooleanVar(value=True)
        tk.Checkbutton(self.root, text="Enable Tracking",
                       variable=self.track_var,
                       command=self.update_track).pack()

        # Custom HUD text
        tk.Label(self.root, text="Custom HUD Text").pack()
        self.custom_text = tk.Entry(self.root)
        self.custom_text.pack(fill="x")
        self.custom_text.bind("<KeyRelease>", self.update_custom_text)

        # Run GUI loop in a thread
        threading.Thread(target=self.root.mainloop, daemon=True).start()

    # Update callbacks
    def update_conf(self, val):
        self.settings["conf"] = float(val)

    def update_iou(self, val):
        self.settings["iou"] = float(val)

    def update_imgsz(self, event):
        self.settings["imgsz"] = int(self.imgsz_var.get())

    def update_hud(self):
        self.settings["hud_enabled"] = self.hud_var.get()

    def update_track(self):
        self.settings["track_enabled"] = self.track_var.get()

    def update_custom_text(self, event):
        self.settings["custom_text"] = self.custom_text.get()