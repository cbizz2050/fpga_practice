import subprocess
import os

class VHDLSimulation:
    def __init__(self, work_dir):
        self.work_dir = work_dir
        self.work_file = "work-obj08.cf"
        self.simulation_running = False
        
    def initialize(self):
        """Initialize VHDL simulation using compiled work library"""
        if not self.simulation_running:
            # Print current working directory and target path for debugging
            print(f"Current working directory: {os.getcwd()}")
            work_path = os.path.join(self.work_dir, self.work_file)
            print(f"Looking for work library at: {work_path}")
            
            # Check if work library exists
            if not os.path.exists(work_path):
                print(f"Available files in {self.work_dir}:")
                try:
                    files = os.listdir(self.work_dir)
                    for f in files:
                        print(f"  {f}")
                except Exception as e:
                    print(f"Error listing directory: {e}")
                raise RuntimeError(f"VHDL work library not found at {work_path}")
            
            # Start GHDL simulation
            try:
                # Add GHDL verification
                result = subprocess.run(['ghdl', '--version'], 
                                     capture_output=True, text=True)
                print(f"GHDL version: {result.stdout}")
                
                self.simulation_running = True
                print("VHDL simulation initialized successfully")
            except Exception as e:
                print(f"Error initializing GHDL: {e}")
                raise

    def update_counters(self, num_enabled):
        """Update the number of enabled counters in the VHDL simulation"""
        if self.simulation_running:
            # Here we would interface with the VHDL simulation
            # For now, we'll just print the action
            print(f"VHDL: Setting enabled counters to {num_enabled}")
            return True
        return False

    def get_counter_status(self):
        """Get the current status of the counters from VHDL simulation"""
        if self.simulation_running:
            # Here we would read from the VHDL simulation
            # For now, return the last known state
            return True
        return False