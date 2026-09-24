# =========================
# 1. IMPORTS
# =========================
import datetime
import re
import time
import ML_Controller
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from typing import Optional
from scipy.interpolate import CubicSpline
from astropy.time import Time
from pygeomag import GeoMag

try:
    from GMATScriptCreator import GMATScriptCreator
except ImportError:
    print("❌ Error: GMATScriptCreator class not found. Make sure GMATScriptCreator.py is in the same directory.")
    exit()

try:
    from HardwareController import HardwareController
except ImportError:
    print("❌ Error: HardwareController class not found. Make sure HardwareController.py is in the same directory.")
    exit()

try:
    from PIController import PIController
except ImportError:
    print("❌ Error: PIController class not found. Make sure PIController.py is in the same directory.")
    exit()

try:
    from DynamicPlotter import DynamicPlotter
except ImportError:
    print("❌ Error: DynamicPlotter class not found. Make sure DynamicPlotter.py is in the same directory.")
    exit()

# =========================
# 2. CONFIGURATION
# =========================
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

# =========================
# 3. HELPER FUNCTIONS
# =========================

def run_gmat_simulation(config: dict) -> tuple[Optional[pd.DataFrame], int]:
    """Runs the GMAT simulation using GMATScriptCreator and loads the report."""
    try:
        gmat_config = {
            'object_name': config['gmat_object_name'],
            'SMA': config['gmat_SMA'],
            'ECC': config['gmat_ECC'],
            'INC': config['gmat_INC'],
            'RAAN': config['gmat_RAAN'],
            'AOP': config['gmat_AOP'],
            'TA': config['gmat_TA'],
            'DryMass': config['gmat_DryMass'],
            'Cd': config['gmat_Cd'],
            'Cr': config['gmat_Cr'],
            'DragArea': config['gmat_DragArea'],
            'simulation_duration_sec': config['gmat_total_duration_sec'],
            'simulation_step_sec': config['gmat_time_step_sec'],
            'script_path': config['gmat_script_path'],
            'report_path': config['gmat_report_filename'],
            'RelativisticCorrection': config['gmat_RelativisticCorrection'],
            'AtmosphereModel': config['gmat_AtmosphereModel'],
        }
        
        gmat_script = GMATScriptCreator(config=gmat_config)
        gmat_script.create_script()
        gmat_script.execute_simulation()
        data = gmat_script.load_mission_report()
        
        total_duration_for_control_loop = config['gmat_total_duration_sec']
        return data, total_duration_for_control_loop
    except Exception as e:
        print(f"❌ Failed to run GMAT simulation: {e}")
        return None, 0

def mjd_to_decimal_year(mjd: float) -> float:
    """Converts a Modified Julian Date to a decimal year."""
    # Reverted to user's original implementation per request.
    jd = mjd + 2430000.0
    t = Time(jd, format='jd')

    calendar_date = t.to_datetime()
    year = calendar_date.year

    start_of_year = datetime.datetime(year, 1, 1)
    next_year = datetime.datetime(year + 1, 1, 1)

    days_in_year = (next_year - start_of_year).days
    days_since_start_of_year = (calendar_date - start_of_year).days + (calendar_date - start_of_year).seconds / 86400.0
    decimal_year = year + days_since_start_of_year / days_in_year
    return decimal_year

def calculate_magnetic_field(df: pd.DataFrame, config: dict) -> pd.DataFrame:
    """Calculates Earth's magnetic field along the satellite's trajectory."""
    print("🌍 Calculating magnetic field values...")
    geo_mag = GeoMag(coefficients_file=config["wmm_coefficients_file"], high_resolution=True)
    
    object_name = config['gmat_object_name']
    utc_col = f"{object_name}.UTCModJulian"
    lat_col = f"{object_name}.Earth.Latitude"
    lon_col = f"{object_name}.Earth.Longitude"
    alt_col = f"{object_name}.Earth.Altitude"
    
    df['Year'] = df[utc_col].apply(mjd_to_decimal_year)

    mag_field_data = df.apply(
        lambda row: geo_mag.calculate(
            glat=row[lat_col],
            glon=row[lon_col],
            alt=row[alt_col],
            time=row['Year']
        ),
        axis=1
    )
    
    df['B_ideal_X'] = [val.x / 1000 for val in mag_field_data] # Convert to µT
    df['B_ideal_Y'] = [val.y / 1000 for val in mag_field_data]
    df['B_ideal_Z'] = [val.z / 1000 for val in mag_field_data]
    
    print("✅ Magnetic field calculations complete.")
    return df

def prepare_control_data(df: pd.DataFrame, config: dict) -> tuple:
    """Applies calibration to calculate target voltages and creates interpolators."""
    print("🔧 Preparing control data and interpolators...")
    df['V_initial_X'] = ((df['B_ideal_X'] - config["cal_cx"]) / config["cal_mx"])
    df['V_initial_Y'] = ((df['B_ideal_Y'] - config["cal_cy"]) / config["cal_my"])
    df['V_initial_Z'] = ((df['B_ideal_Z'] - config["cal_cz"]) / config["cal_mz"]) 

    time_points = np.arange(0, len(df) * config['gmat_time_step_sec'], config['gmat_time_step_sec'])

    if len(time_points) > len(df):
        time_points = time_points[:len(df)]

    cs_b_x = CubicSpline(time_points, df['B_ideal_X'])
    cs_b_y = CubicSpline(time_points, df['B_ideal_Y'])
    cs_b_z = CubicSpline(time_points, df['B_ideal_Z'])
    
    cs_v_x = CubicSpline(time_points, df['V_initial_X'])
    cs_v_y = CubicSpline(time_points, df['V_initial_Y'])
    cs_v_z = CubicSpline(time_points, df['V_initial_Z'])
    
    print("✅ Control data ready.")
    return cs_b_x, cs_b_y, cs_b_z, cs_v_x, cs_v_y, cs_v_z

def plot_results(ideal_data: list, real_data: list, time_steps: list):
    """Plots the ideal vs. real magnetic field data."""
    if not ideal_data or not real_data:
        print("⚠️ No data to plot.")
        return
        
    ideal = np.array(ideal_data)
    real = np.array(real_data)
    
    fig, axs = plt.subplots(3, 1, figsize=(12, 10), sharex=True)
    fig.suptitle('Magnetic Field Simulation: Ideal vs. Real', fontsize=16)
    
    labels = ['X', 'Y', 'Z']
    for i, ax in enumerate(axs):
        ax.plot(time_steps, ideal[:, i], label=f'Ideal {labels[i]}', color='blue', linewidth=2)
        ax.plot(time_steps, real[:, i], label=f'Real (Measured) {labels[i]}', color='orange', linestyle='--', linewidth=1.5)
        ax.set_ylabel(f'B$_{{{labels[i].lower()}}}$ (µT)')
        ax.legend()
        ax.grid(True, linestyle=':')
        
    axs[-1].set_xlabel('Simulation Time (s)')
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.show()

def simulate_gmat_data_and_plot():
    """Main function to run the entire magnetic field simulation and control loop."""
    gmat_data, total_duration = run_gmat_simulation(CONFIG)
    if gmat_data is None or gmat_data.empty:
        print("❌ Halting execution due to GMAT simulation failure.")
        return
        
    full_data = calculate_magnetic_field(gmat_data, CONFIG)
    cs_b_x, cs_b_y, cs_b_z, cs_v_x, cs_v_y, cs_v_z = prepare_control_data(full_data, CONFIG)
    
    controller = HardwareController(CONFIG)
    pi_x = PIController(Kp=CONFIG["control_kp"], Ki=CONFIG["control_ki"])
    pi_y = PIController(Kp=CONFIG["control_kp"], Ki=CONFIG["control_ki"])
    pi_z = PIController(Kp=CONFIG["control_kp"], Ki=CONFIG["control_ki"])

    plotter = DynamicPlotter(window_size=200)

    v_adj_x, v_adj_y, v_adj_z = 0.0, 0.0, 0.0
    last_real_mag = (0, 0, 0)
    
    try:
        controller.setup()
        
        print("\n▶️ Starting continuous control loop...")
        simulation_time = 0.0

        target_loop_duration_real_time = CONFIG["simulation_dt"] / CONFIG["speed_multiplier"]

        while simulation_time <= total_duration:
            # Record the real time at the start of the loop
            loop_start_real_time = time.time()

            ideal_mag_x = cs_b_x(simulation_time)
            ideal_mag_y = cs_b_y(simulation_time)
            ideal_mag_z = cs_b_z(simulation_time)
            
            v_ideal_x = cs_v_x(simulation_time)
            v_ideal_y = cs_v_y(simulation_time)
            v_ideal_z = cs_v_z(simulation_time)

            v_applied_x = v_ideal_x + v_adj_x
            v_applied_y = v_ideal_y + v_adj_y
            v_applied_z = v_ideal_z + v_adj_z
            
            controller.set_field_voltages(v_applied_x, v_applied_y, v_applied_z)

            real_mag_reading = controller.read_magnetometer()
            
            if real_mag_reading:
                real_mag_x, real_mag_y, real_mag_z = real_mag_reading
                last_real_mag = real_mag_reading
                
                v_adj_x = pi_x.calculate(ideal_mag_x, real_mag_x)
                v_adj_y = pi_y.calculate(ideal_mag_y, real_mag_y)
                v_adj_z = pi_z.calculate(ideal_mag_z, real_mag_z)
            else:
                print(f"       | ⚠️ Missed magnetometer reading. Re-using last known values.")
                real_mag_x, real_mag_y, real_mag_z = last_real_mag

            # --- Update Plot ---
            plotter.update(
                simulation_time,
                (ideal_mag_x, ideal_mag_y, ideal_mag_z),
                (real_mag_x, real_mag_y, real_mag_z),
                (v_applied_x, v_applied_y, v_applied_z)
            )
            
            # --- Time Management with Fixed Step ---
            simulation_time += CONFIG["simulation_dt"]

            real_time_elapsed_in_loop = time.time() - loop_start_real_time
            sleep_duration = target_loop_duration_real_time - real_time_elapsed_in_loop
            if sleep_duration > 0:
                time.sleep(sleep_duration)

        print("\n⏹️ Control loop finished.")

    except (Exception, KeyboardInterrupt) as e:
        print(f"\n❌ An error occurred during the main loop: {e}")
        print("   Attempting graceful shutdown...")
    finally:
        controller.shutdown()
        print("\n📊 Finalizing plot...")
        plotter.close()

def simulate_rotating_magnet():
    """
    Generates a rotating magnetic field in the XY plane for a fixed duration.

    This function bypasses the GMAT simulation and directly controls the Helmholtz
    cage to produce a sinusoidal magnetic field. It's useful for testing
    the system's response and calibration. The target (ideal) and measured
    (real) magnetic fields are plotted dynamically.
    """
    print("🔄 Starting rotating magnet simulation...")

    # --- Simulation Parameters ---
    SIM_DURATION_S = 60      # Total duration of the simulation in seconds
    TIME_STEP_S = 0.1        # Time step for the control loop in seconds
    AMPLITUDE_UT = 160.0      # Amplitude of the magnetic field in microTeslas (µT)
    FREQUENCY_HZ = 1       # Frequency of rotation in Hertz (results in a 10-second period)

    # --- Initialize Components ---
    try:
        controller = HardwareController(CONFIG)
        plotter = DynamicPlotter(window_size=200)
    except NameError as e:
        print(f"❌ Error: A required class is not defined. {e}")
        return
    
    last_real_mag = (0, 0, 0)
    
    try:
        controller.setup()
        print(f"▶️ Running for {SIM_DURATION_S} seconds. Press Ctrl+C to stop early.")
        
        current_time = 0.0

        while current_time < SIM_DURATION_S:
            loop_start_real_time = time.time()

            # --- Calculate Target B-Field (Ideal) ---
            angle = 2 * np.pi * FREQUENCY_HZ * current_time
            ideal_mag_x = AMPLITUDE_UT * np.cos(angle)
            ideal_mag_y = AMPLITUDE_UT * np.sin(angle)
            ideal_mag_z = 0.0  # Keep the Z-axis field at zero

            # --- Calculate Required Voltages ---
            v_applied_x = (ideal_mag_x - CONFIG["cal_cx"]) / CONFIG["cal_mx"]
            v_applied_y = (ideal_mag_y - CONFIG["cal_cy"]) / CONFIG["cal_my"]
            v_applied_z = (ideal_mag_z - CONFIG["cal_cz"]) / CONFIG["cal_mz"]
            
            # --- Control Hardware ---
            controller.set_field_voltages(v_applied_x, v_applied_y, v_applied_z)

            # --- Read Measured B-Field (Real) ---
            real_mag_reading = controller.read_magnetometer()
            
            if real_mag_reading:
                real_mag_x, real_mag_y, real_mag_z = real_mag_reading
                last_real_mag = real_mag_reading
            else:
                real_mag_x, real_mag_y, real_mag_z = last_real_mag
                print("⚠️ Missed magnetometer reading, using last known value.")

            # --- Update Plot ---
            plotter.update(
                current_time,
                (ideal_mag_x, ideal_mag_y, ideal_mag_z),
                (real_mag_x, real_mag_y, real_mag_z),
                (v_applied_x, v_applied_y, v_applied_z)
            )
            
            # --- Time Management ---
            current_time += TIME_STEP_S
            
            real_time_elapsed = time.time() - loop_start_real_time
            sleep_duration = TIME_STEP_S - real_time_elapsed
            if sleep_duration > 0:
                time.sleep(sleep_duration)

        print("\n⏹️ Simulation finished.")

    except (Exception, KeyboardInterrupt) as e:
        print(f"\n❌ An error occurred or simulation was interrupted: {e}")
    finally:
        print("🔌 Shutting down hardware and closing plot...")
        controller.shutdown()
        plotter.close()
        print("✅ Shutdown complete.")
    
def simulate_gmat_data_and_visualize(CONFIG, model="None"):
    gmat_data, total_duration = run_gmat_simulation(CONFIG)
    if gmat_data is None or gmat_data.empty:
        print("❌ Halting execution due to GMAT simulation failure.")
        return
        
    full_data = calculate_magnetic_field(gmat_data, CONFIG)
    cs_b_x, cs_b_y, cs_b_z, cs_v_x, cs_v_y, cs_v_z = prepare_control_data(full_data, CONFIG)
    
    controller = HardwareController(CONFIG)

    last_real_mag = (0, 0, 0)
    results = []

    if model != "None":
        ML = ML_Controller.ML(model_name=model)
    
    try:
        controller.setup()
        
        print("\n▶️ Starting continuous control loop...")
        simulation_time = 0.0

        target_loop_duration_real_time = CONFIG["simulation_dt"]
        target_total_duration = total_duration / CONFIG["speed_multiplier"]

        while simulation_time <= target_total_duration:
            # Record the real time at the start of the loop
            loop_start_real_time = time.time()

            ideal_mag_x = cs_b_x(simulation_time*CONFIG["speed_multiplier"])
            ideal_mag_y = cs_b_y(simulation_time*CONFIG["speed_multiplier"])
            ideal_mag_z = cs_b_z(simulation_time*CONFIG["speed_multiplier"])

            if model != "None":
                v_ideal_x, v_ideal_y, v_ideal_z = ML.get_prediction(ideal_mag_x, ideal_mag_y, ideal_mag_z)
            else:
                v_ideal_x = cs_v_x(simulation_time*CONFIG["speed_multiplier"])
                v_ideal_y = cs_v_y(simulation_time*CONFIG["speed_multiplier"])
                v_ideal_z = cs_v_z(simulation_time*CONFIG["speed_multiplier"])
            

            controller.set_field_voltages(v_ideal_x, v_ideal_y, v_ideal_z)

            real_mag_reading = controller.read_magnetometer()
            
            if real_mag_reading:
                real_mag_x, real_mag_y, real_mag_z = real_mag_reading
                last_real_mag = real_mag_reading

            else:
                print(f"⚠️ Missed magnetometer reading. Re-using last known values.")
                real_mag_x, real_mag_y, real_mag_z = last_real_mag

            # Save: time, real B (µT), ideal B (µT), applied V (V)
            results.append((
                time.time(),
                real_mag_x, real_mag_y, real_mag_z,
                ideal_mag_x, ideal_mag_y, ideal_mag_z,
                v_ideal_x, v_ideal_y, v_ideal_z
            ))
            # --- Time Management with Fixed Step ---
            simulation_time += CONFIG["simulation_dt"]

            real_time_elapsed_in_loop = time.time() - loop_start_real_time
            sleep_duration = target_loop_duration_real_time - real_time_elapsed_in_loop
            if sleep_duration > 0:
                time.sleep(sleep_duration)

        print("\n⏹️ Control loop finished.")

    except (Exception, KeyboardInterrupt) as e:
        print(f"\n❌ An error occurred during the main loop: {e}")
        print("   Attempting graceful shutdown...")
    finally:
        controller.shutdown()
        
        results = np.array(results)
        times = results[:, 0] - results[0, 0]
        RMx, RMy, RMz = results[:, 1], results[:, 2], results[:, 3]  # Real B (µT)
        IMx, IMy, IMz = results[:, 4], results[:, 5], results[:, 6]  # Ideal B (µT)
        Vx,  Vy,  Vz  = results[:, 7], results[:, 8], results[:, 9]  # Applied V (V)

        # Plot fields (same graph per axis)
        fig, axs = plt.subplots(3, 1, figsize=(12, 10), sharex=True)
        fig.suptitle('Magnetic Field Simulation: Ideal vs. Real', fontsize=16)
        data_pairs = [(IMx, RMx, 'X'), (IMy, RMy, 'Y'), (IMz, RMz, 'Z')]
        for ax, (ideal, real, lbl) in zip(axs, data_pairs):
            ax.plot(times, ideal, label=f'Ideal {lbl}', color='blue', linewidth=2)
            ax.plot(times, real,  label=f'Real {lbl}',  color='orange', linestyle='--', linewidth=1.5)
            ax.set_ylabel(f'B_{lbl} (µT)')
            ax.legend(); ax.grid(True, linestyle=':')
        axs[-1].set_xlabel('Time (s)')
        plt.tight_layout(rect=[0, 0, 1, 0.96])
        plt.show()

        # Optional: plot applied voltages with correct units
        plt.figure(figsize=(12, 6))
        plt.plot(times, Vx, label='Vx'); plt.plot(times, Vy, label='Vy'); plt.plot(times, Vz, label='Vz')
        plt.title('Applied Voltages'); plt.ylabel('Voltage (V)'); plt.xlabel('Time (s)'); plt.grid(True); plt.legend()
        plt.show()


def get_average_mag_reading(controller: HardwareController, samples: int = 10, delay: float = 0.1) -> tuple[float, float, float]:
    """Averages multiple magnetometer readings to reduce noise."""
    readings = []
    for _ in range(samples):
        reading = controller.read_magnetometer()
        if reading:
            readings.append(reading)
        time.sleep(delay)
    
    if readings:
        avg_reading = np.mean(readings, axis=0)
        return tuple(avg_reading)
    else:
        return (0.0, 0.0, 0.0)

def generate_simulation_voltages_permutations(max_voltage, step_size):
    """Generates all combinations of voltages from -max_voltage to +max_voltage with given step size."""
    voltages = np.arange(-max_voltage, max_voltage + step_size, step_size)
    for vx in voltages:
        for vy in voltages:
            for vz in voltages:
                yield (vx, vy, vz)

def generate_simulation_voltages_linear(max_voltage, step):
    x = np.arange(-max_voltage, max_voltage + step, step)
    y = np.arange(-max_voltage, max_voltage + step, step)
    z = np.arange(-max_voltage, max_voltage + step, step)

    simulation = []
    for volt_x in x:
        simulation.append((volt_x, 0.0, 0.0))
    for volt_y in y:
        simulation.append((0.0, volt_y, 0.0))
    for volt_z in z:
        simulation.append((0.0, 0.0, volt_z))

    return simulation

def generate_simulation_voltages_all_permutations(max_voltage, step):
    voltages = np.arange(-max_voltage, max_voltage + step, step)
    simulation = []
    for vx in voltages:
        for vy in voltages:
            for vz in voltages:
                simulation.append((vx, vy, vz))
    return simulation

def generate_simulation_voltages_all_permutations_resume():
    simulation = []

    for vy in range(16, 21, 1):
        for vz in range(-20, 21, 1):
            simulation.append((16, vy, vz))

    for vx in range(17, 21, 1):
        for vy in range(-20, 21, 1):
            for vz in range(-20, 21, 1):
                simulation.append((vx, vy, vz))

    
    return simulation
def test_power_sources(CONFIG, mean = True):
    print("🔌 Testing power supplies...")
    simulation = generate_simulation_voltages_all_permutations_resume()
    # simulation = generate_simulation_voltages_all_permutations(max_voltage=20.0, step=1)

    controller = HardwareController(CONFIG)
    controller.setup()
    controller.set_relay_state("011") # Initial, positive X, Y and Z

    results = []

    try:
        # Write header once before the loop
        with open('sim_lineal.csv', 'w') as f:
            f.write("Time,Mx,My,Mz,Vx,Vy,Vz\n")

        for voltages in simulation:
            controller.set_field_voltages(voltages[0], voltages[1], voltages[2])
            if mean:
                time.sleep(1) # Wait for stabilization

                mag_reading = get_average_mag_reading(controller, samples=10, delay=0.1)
                result = (time.time(), mag_reading[0], mag_reading[1], mag_reading[2], voltages[0], voltages[1], voltages[2])
                results.append(result)
                # Save to CSV in each iteration
                with open('sim_lineal.csv', 'a') as f:
                    f.write(','.join(map(str, result)) + '\n')
        
            else:
                for _ in range(20):
                    mag_reading = controller.read_magnetometer()
                    time.sleep(0.1)
                    result = (time.time(), mag_reading[0], mag_reading[1], mag_reading[2], voltages[0], voltages[1], voltages[2])
                    results.append(result)
                    # Save to CSV in each iteration
                    with open('sim_lineal.csv', 'a') as f:
                        f.write(','.join(map(str, result)) + '\n')
    except Exception as e:
        print("❌ An error occurred during the power supply test.")
        print(f"   Error details: {e}")
    finally:
        controller.shutdown()


    results = np.array(results)
    times = results[:, 0] - results[0, 0]  # Relative time
    Mx, My, Mz = results[:, 1], results[:, 2], results[:, 3]
    Vx, Vy, Vz = results[:, 4], results[:, 5], results[:, 6]

    fig, axs = plt.subplots(2, 1, figsize=(10, 8), sharex=True)
    fig.suptitle('Power Source Test Results', fontsize=16)

    axs[0].plot(times, Vx, 'r-o', label='Vx')
    axs[0].plot(times, Vy, 'g-o', label='Vy')
    axs[0].plot(times, Vz, 'b-o', label='Vz')
    axs[0].set_ylabel('Applied Voltage')
    axs[0].legend()
    axs[0].grid(True)

    axs[1].plot(times, Mx, 'r--o', label='Mx')
    axs[1].plot(times, My, 'g--o', label='My')
    axs[1].plot(times, Mz, 'b--o', label='Mz')
    axs[1].set_ylabel('Measured Magnetic Field')
    axs[1].set_xlabel('Time (s)')
    axs[1].legend()
    axs[1].grid(True)

    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.show()
