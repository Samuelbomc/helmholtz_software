import time
import serial
import pyvisa
from typing import Optional

class HardwareController:
    """
    Manages all hardware interactions: power supplies and Arduinos.
    Encapsulates initialization, control, and shutdown logic.
    """
    def __init__(self, config: dict):
        self.config = config
        self.rm = pyvisa.ResourceManager()
        self.instrument_x = None
        self.instrument_y = None
        self.instrument_z = None
        self.arduino_magnetometer = None
        self.arduino_relay = None
        self.current_relay_state = "000" # Initial state assumes positive voltage

    def setup(self):
        """Initializes and configures all hardware components."""
        print("🔌 Initializing hardware...")
        try:
            # Connect to Power Supplies
            self.instrument_x = self._connect_instrument(self.config["power_supply_x_addr"])
            self.instrument_y = self._connect_instrument(self.config["power_supply_y_addr"])
            self.instrument_z = self._connect_instrument(self.config["power_supply_z_addr"])

            # Connect to Arduinos
            self.arduino_magnetometer = serial.Serial(port=self.config["magnetometer_port"], baudrate=self.config['mag_baud_rate'], timeout=self.config["serial_timeout"])
            self.arduino_relay = serial.Serial(port=self.config["relay_port"], baudrate=self.config['relay_baud_rate'], timeout=self.config["serial_timeout"])
            time.sleep(2) # Wait for serial ports to initialize

            # Configure Power Supplies
            self._configure_power_supply(self.instrument_x, "X")
            self._configure_power_supply(self.instrument_y, "Y")
            self._configure_power_supply(self.instrument_z, "Z")
            
            self.set_relay_state(self.current_relay_state) # Set initial state
            print("✅ Hardware setup complete.")

        except Exception as e:
            print(f"❌ Error during hardware setup: {e}")
            self.shutdown() 
            raise

    def _connect_instrument(self, address: str) -> pyvisa.Resource:
        """Opens a connection to a VISA instrument."""
        instrument = self.rm.open_resource(
            address, 
            baud_rate=self.config["baud_rate"], 
            timeout=self.config["pyvisa_timeout"]
        )
        instrument.write_termination = '\n'
        instrument.read_termination = '\n'
        return instrument

    def _configure_power_supply(self, instrument: pyvisa.Resource, axis: str):
        """Applies initial settings to a power supply."""
        idn = instrument.query('*IDN?')
        print(f"   - Instrument {axis} ID: {idn.strip()}")
        instrument.write(f':SOUR1:VOLT 0.0')
        instrument.write(f':SOUR1:CURR {self.config["max_current"]}')
        instrument.write('OUTP1:STAT ON')

    def set_field_voltages(self, vx: float, vy: float, vz: float):
        """Sets the output voltages and relay states for the magnetic field coils."""
        new_relay_state = ''.join(['0' if v < 0 else '1' for v in [-vx, vy, vz]]) # Invert X for relay logic
        
        if new_relay_state != self.current_relay_state:
            self.set_relay_state(new_relay_state)
            self.current_relay_state = new_relay_state

        self.instrument_x.write(f':SOUR1:VOLT {float(abs(vx)):.4f}')
        self.instrument_y.write(f':SOUR1:VOLT {float(abs(vy)):.4f}')
        self.instrument_z.write(f':SOUR1:VOLT {float(abs(vz)):.4f}')

    def set_relay_state(self, state: str):
        """Sends a 3-bit state ('000' to '111') to the relay Arduino."""
        if len(state) == 3 and all(c in '01' for c in state):
            self.arduino_relay.write(f"{state}\n".encode())
            pass
        else:
            print(f"⚠️ Warning: Invalid relay state requested: {state}")

    def read_magnetometer(self) -> Optional[tuple[float, float, float]]:
        """
        Reads and parses a single line from the magnetometer Arduino.
        This version is more robust, attempting a few reads if the first one fails.
        """
        self.arduino_magnetometer.reset_input_buffer()
        for _ in range(3):
            try:
                line = self.arduino_magnetometer.readline().decode('utf-8').strip()
                if not line:
                    time.sleep(0.01)
                    continue
                    
                parts = line.split(',')
                if len(parts) == 3:
                    try:
                        b_x = float(parts[0].strip())
                        b_y = float(parts[1].strip())
                        b_z = float(parts[2].strip())
                        return (b_x, b_y, b_z)
                    except ValueError:
                        continue 
            except (IndexError, UnicodeDecodeError):
                continue
        
        return None


    def shutdown(self):
        """Turns off and closes all hardware connections safely."""
        print("🔌 Shutting down hardware...")
        for inst_name in ['x', 'y', 'z']:
            try:
                instrument = getattr(self, f"instrument_{inst_name}")
                if instrument:
                    instrument.write('OUTP1:STAT OFF')
                    instrument.close()
            except Exception as e:
                print(f"   - Error shutting down instrument {inst_name.upper()}: {e}")
        
        for port_name in ['relay', 'magnetometer']:
            try:
                port = getattr(self, f"arduino_{port_name}")
                if port and port.is_open:
                    port.close()
            except Exception as e:
                print(f"   - Error closing {port_name} serial port: {e}")
        print("✅ Hardware shutdown complete.")