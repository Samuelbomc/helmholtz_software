import tkinter as tk
from tkinter import messagebox
import csv
import os

CONFIG = {
    # ---------------------------------
    # GMAT Simulation Settings
    # ---------------------------------
    # Orbital Elements & Spacecraft Properties
    'gmat_object_name': 'TestSat',
    'gmat_SMA': 6800.895,      # Semimajor Axis (km)
    'gmat_ECC': 0.0010563,     # Eccentricity
    'gmat_INC': 51.57977,      # Inclination (deg)
    'gmat_RAAN': 327.87345,    # Right Ascension of the Ascending Node (deg)
    'gmat_AOP': 43.91870,      # Argument of Perigee (deg)
    'gmat_TA': 315.92945,      # True Anomaly (deg)
    'gmat_DryMass': 450,       # kg
    'gmat_Cd': 2.2,            # Drag Coefficient
    'gmat_Cr': 1.8,            # Radiation Pressure Coefficient
    'gmat_DragArea': 25.0,     # m^2

    # Simulation Timing & Reporting
    "gmat_total_duration_sec": 100000, # Total simulation time
    "gmat_time_step_sec": 600,        # Data reporting step
    "gmat_script_path": "./data", # Script and report directory
    "gmat_report_filename": "GMAT-Mission-Report.txt", # Report file name

    # Force Model & Environment
    'gmat_RelativisticCorrection': 'Off',
    'gmat_AtmosphereModel': 'MSISE90',
    "wmm_coefficients_file": "WMMHR.COF", # World Magnetic Model file

    # ---------------------------------
    # Hardware Settings
    # ---------------------------------
    "power_supply_x_addr": 'ASRL5::INSTR',
    "power_supply_y_addr": 'ASRL6::INSTR',
    "power_supply_z_addr": 'ASRL7::INSTR',
    "magnetometer_port": 'COM3',
    "relay_port": 'COM4',
    "baud_rate": 115200, # Baud rate for power supplies
    "mag_baud_rate": 9600, # Baud rate for magnetometer Arduino
    "relay_baud_rate": 9600, # Baud rate for relay Arduino
    "pyvisa_timeout": 2000, # ms
    "serial_timeout": 0.05,     # s
    "max_current": 3.4,      # Amperes
    
    # ---------------------------------
    # Calibration & Control Settings
    # ---------------------------------
    # Voltage-to-Magnetic-Field Calibration Constants (V = m*B + c)
    # Adjusted based on the latest plot to better fit the observed data,
    # accounting for hardware issues like inverted polarity on the Y-axis.
    "cal_mx": 8.0123, "cal_cx": 10.077,
    "cal_my": 6.5485, "cal_cy": 9.5105,
    "cal_mz": 9.6051, "cal_cz": 11.013,

    # Control Loop Timing
    "speed_multiplier": 6000, # How many simulation seconds pass per real second.
    "simulation_dt": 0.1, # Simulation time step in seconds.

    # PI Controller Settings (Gains reduced to prevent saturation)
    "control_kp": 0.1,
    "control_ki": 0,
}

def create_config():
    """
    Create a UI with all configuration settings on one screen for user input.
    
    This window displays all keys from the CONFIG dictionary in a scrollable
    area and provides entry fields for the user to modify their values. It also
    allows saving to and loading from a CSV file.
    
    Returns:
        dict: The updated configuration settings. If the window is closed
              without saving, the original default settings are returned.
    """
    root = tk.Tk()
    root.title("Helmholtz Control System Configuration")
    root.geometry("500x600") 

    final_config = CONFIG.copy()
    entries = {}

    # --- File I/O Setup ---
    data_dir = "data"
    csv_path = os.path.join(data_dir, "config.csv")
    
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)

    # --- Create a scrollable frame ---
    main_frame = tk.Frame(root)
    main_frame.pack(fill=tk.BOTH, expand=1)

    canvas = tk.Canvas(main_frame)
    canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=1)

    scrollbar = tk.Scrollbar(main_frame, orient=tk.VERTICAL, command=canvas.yview)
    scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

    canvas.configure(yscrollcommand=scrollbar.set)
    
    scrollable_frame = tk.Frame(canvas, padx=15, pady=15)
    canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")

    scrollable_frame.bind(
        "<Configure>",
        lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
    )
    
    def _on_mousewheel(event):
        scroll_speed = -1 if event.num == 4 or event.delta > 0 else 1
        canvas.yview_scroll(scroll_speed, "units")

    canvas.bind_all("<MouseWheel>", _on_mousewheel)
    canvas.bind_all("<Button-4>", _on_mousewheel)
    canvas.bind_all("<Button-5>", _on_mousewheel)

    # --- Dynamically create widgets inside the scrollable_frame ---
    for i, (key, value) in enumerate(CONFIG.items()):
        label_text = key.replace('_', ' ').title()
        label = tk.Label(scrollable_frame, text=f"{label_text}:")
        label.grid(row=i, column=0, sticky="w", padx=5, pady=5)

        entry = tk.Entry(scrollable_frame, width=35)
        entry.grid(row=i, column=1, padx=5, pady=5)
        entry.insert(0, str(value))
        entries[key] = entry
    
    def get_current_values_from_ui():
        """Helper function to read and validate all values from the UI."""
        temp_config = {}
        for key, entry_widget in entries.items():
            value_str = entry_widget.get()
            original_value = CONFIG[key]

            if isinstance(original_value, int):
                temp_config[key] = int(value_str)
            elif isinstance(original_value, float):
                temp_config[key] = float(value_str)
            elif isinstance(original_value, str) and original_value.lower() in ['true', 'false']:
                 temp_config[key] = value_str.title()
            else:
                temp_config[key] = value_str
        return temp_config

    def save_to_csv():
        """Saves the current UI configuration to a CSV file."""
        try:
            current_config = get_current_values_from_ui()
            with open(csv_path, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(['key', 'value']) # Write header
                for key, value in current_config.items():
                    writer.writerow([key, value])
            messagebox.showinfo("Success", f"Configuration saved to {csv_path}")
        except ValueError:
            messagebox.showerror("Invalid Input", "Cannot save. Please ensure all numerical fields have valid numbers.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save file: {e}")

    def load_from_csv():
        """Loads configuration from a CSV file into the UI."""
        if not os.path.exists(csv_path):
            messagebox.showerror("Error", f"Configuration file not found at {csv_path}")
            return
        
        try:
            with open(csv_path, 'r') as f:
                reader = csv.reader(f)
                next(reader) # Skip header
                loaded_config = {rows[0]: rows[1] for rows in reader}

            for key, value in loaded_config.items():
                if key in entries:
                    entries[key].delete(0, tk.END)
                    entries[key].insert(0, value)
            
            messagebox.showinfo("Success", f"Configuration loaded from {csv_path}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load file: {e}")

    def save_and_close():
        """Validates UI values, updates the final_config dict, and closes the window."""
        try:
            current_config = get_current_values_from_ui()
            final_config.clear()
            final_config.update(current_config)
            
            messagebox.showinfo("Success", "Configuration has been set.")
            root.destroy()
        except ValueError:
            messagebox.showerror("Invalid Input", "Please ensure all numerical fields have valid numbers.")

    # --- UI Controls  ---
    button_frame = tk.Frame(root, pady=10)
    button_frame.pack(fill=tk.X)
    
    center_button_frame = tk.Frame(button_frame)
    center_button_frame.pack()

    load_button = tk.Button(center_button_frame, text="Load from CSV", command=load_from_csv)
    load_button.pack(side=tk.LEFT, padx=5)

    save_csv_button = tk.Button(center_button_frame, text="Save to CSV", command=save_to_csv)
    save_csv_button.pack(side=tk.LEFT, padx=5)

    save_close_button = tk.Button(center_button_frame, text="Save and Close", command=save_and_close)
    save_close_button.pack(side=tk.LEFT, padx=5)

    root.wait_window()
    
    return final_config
