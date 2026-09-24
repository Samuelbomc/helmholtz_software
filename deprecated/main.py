from helmholtz_control import *
from helmholtz_UI import *

def main():
   # First create the CONFIG data via a user interface
   config = create_config()
   # simulate_rotating_magnet()
   # simulate_gmat_data_and_plot()
   # test_power_sources(CONFIG=config)
   simulate_gmat_data_and_visualize(CONFIG=config, model="NN")

if __name__ == "__main__":
    main()