import matplotlib.pyplot as plt
from collections import deque

class DynamicPlotter:
    """
    Class to manage the dynamic simulation data plotting in real time.
    """
    def __init__(self, window_size=200):
        """
        Initializes the figure, axes, and graph lines.
        
        Args:
            window_size (int): The number of data points to display in the plot.
        """
        plt.ion()
        
        # Create the figure and two subplots (one for magnetic field, another for voltage)
        self.fig, (self.ax1, self.ax2) = plt.subplots(2, 1, figsize=(12, 8), sharex=True)
        self.window_size = window_size

        self.time = deque(maxlen=window_size)
        self.ideal_b = {'x': deque(maxlen=window_size), 'y': deque(maxlen=window_size), 'z': deque(maxlen=window_size)}
        self.real_b = {'x': deque(maxlen=window_size), 'y': deque(maxlen=window_size), 'z': deque(maxlen=window_size)}
        self.voltage = {'x': deque(maxlen=window_size), 'y': deque(maxlen=window_size), 'z': deque(maxlen=window_size)}
        
        # --- Magnetic field subplot configuration ---
        self.ax1.set_title("Magnetic Field: Ideal vs. Measured")
        self.ax1.set_ylabel("Magnetic Field (T)")
        self.lines_b_ideal = {
            'x': self.ax1.plot([], [], '--', label='Ideal B X', color='C0')[0],
            'y': self.ax1.plot([], [], '--', label='Ideal B Y', color='C1')[0],
            'z': self.ax1.plot([], [], '--', label='Ideal B Z', color='C2')[0]
        }
        self.lines_b_real = {
            'x': self.ax1.plot([], [], '-', label='Measured B X', color='C0', lw=2)[0],
            'y': self.ax1.plot([], [], '-', label='Measured B Y', color='C1', lw=2)[0],
            'z': self.ax1.plot([], [], '-', label='Measured B Z', color='C2', lw=2)[0]
        }
        self.ax1.legend(loc='upper left')
        self.ax1.grid(True)

        # --- Applied Voltage subplot configuration ---
        self.ax2.set_title("Applied Voltage to Coils")
        self.ax2.set_xlabel("Simulation Time (s)")
        self.ax2.set_ylabel("Voltage (V)")
        self.lines_v = {
            'x': self.ax2.plot([], [], '-', label='Voltage X', color='C3')[0],
            'y': self.ax2.plot([], [], '-', label='Voltage Y', color='C4')[0],
            'z': self.ax2.plot([], [], '-', label='Voltage Z', color='C5')[0]
        }
        self.ax2.legend(loc='upper left')
        self.ax2.grid(True)

        # Show the figure without blocking execution
        plt.tight_layout()
        plt.show(block=False)

    def update(self, sim_time, ideal_b_vals, real_b_vals, voltage_vals):
        """
        Adds new data points and updates the plot lines.
        """
        # Add the new data to the deque collections
        self.time.append(sim_time)
        
        # Unpack and add the field and voltage values
        b_ideal_x, b_ideal_y, b_ideal_z = ideal_b_vals
        b_real_x, b_real_y, b_real_z = real_b_vals
        v_x, v_y, v_z = voltage_vals

        self.ideal_b['x'].append(b_ideal_x)
        self.ideal_b['y'].append(b_ideal_y)
        self.ideal_b['z'].append(b_ideal_z)
        
        self.real_b['x'].append(b_real_x)
        self.real_b['y'].append(b_real_y)
        self.real_b['z'].append(b_real_z)

        self.voltage['x'].append(v_x)
        self.voltage['y'].append(v_y)
        self.voltage['z'].append(v_z)

        # Update the data for each line in the plot
        for axis in ['x', 'y', 'z']:
            self.lines_b_ideal[axis].set_data(self.time, self.ideal_b[axis])
            self.lines_b_real[axis].set_data(self.time, self.real_b[axis])
            self.lines_v[axis].set_data(self.time, self.voltage[axis])

        # Rescale the axes
        self.ax1.relim()
        self.ax1.autoscale_view()
        self.ax2.relim()
        self.ax2.autoscale_view()
        
        # Redraw the figure canvas
        self.fig.canvas.draw()
        self.fig.canvas.flush_events()

    def close(self):
        """
        Keeps the plot window open after the script finishes.
        """
        plt.ioff() # Disable interactive mode
        print("\n✅ Plot finished. Close the plot window to end the program.")
        plt.show() # Show the window in blocking mode