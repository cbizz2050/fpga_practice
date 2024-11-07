import socket
import os
import struct
import threading
import logging
from register_map import RegisterMap, FPGARegisters
from ghdl_interface import VHDLSimulation

logging.basicConfig(
    level=logging.INFO,
    format='[FPGA Server] %(asctime)s - %(message)s',
    datefmt='%H:%M:%S'
)

class FPGAServer:
    def __init__(self):
        self.socket_path = "/tmp/fpga_sim.sock"
        self.register_map = RegisterMap()
        
        # Use the correct path to work-obj08.cf
        work_dir = "/app/counter_vhdl/build"
        print(f"Initializing VHDL simulation with work directory: {work_dir}")
        self.vhdl_sim = VHDLSimulation(work_dir)
        self.running = False
        # Initialize registers
        self.register_map.write_reg(FPGARegisters.CONFIG_REG, 256)  # MAX_COUNTERS
        self.register_map.write_reg(FPGARegisters.STATUS_REG, 0)    # No counters enabled
        self.register_map.write_reg(FPGARegisters.ENABLE_REG, 0)    # All counters disabled


    def handle_connection(self, conn):
        """Handle client connection"""
        try:
            while True:
                # Read command (1 byte) and address (4 bytes)
                cmd_data = conn.recv(5)
                if not cmd_data:
                    break

                cmd = cmd_data[0]
                addr = struct.unpack("!I", cmd_data[1:5])[0]

                if cmd == 1:  # Write
                    # Read 4 bytes of data
                    data = struct.unpack("!I", conn.recv(4))[0]
                    self.handle_write(addr, data)
                    conn.send(struct.pack("!B", 0))  # Success
                elif cmd == 0:  # Read
                    value = self.handle_read(addr)
                    conn.send(struct.pack("!I", value))
        finally:
            conn.close()

    def handle_write(self, addr, data):
        """Handle register write"""
        self.register_map.write_reg(addr, data)
        
        if addr == FPGARegisters.ENABLE_REG:
            # Count number of enabled bits for status
            enabled_count = bin(data).count('1')
            self.register_map.write_reg(FPGARegisters.STATUS_REG, enabled_count)
            logging.info(f"Updated enabled counters to: {enabled_count}")
            self.vhdl_sim.update_counters(data)
        elif addr == FPGARegisters.RESET_REG and data == 1:
            # On reset, clear enable and status registers
            self.register_map.write_reg(FPGARegisters.ENABLE_REG, 0)
            self.register_map.write_reg(FPGARegisters.STATUS_REG, 0)
            logging.info("Reset FPGA state")
            self.vhdl_sim.update_counters(0)

    def handle_read(self, addr):
        """Handle register read"""
        if addr == FPGARegisters.STATUS_REG:
            # We should return the actual number of enabled counters
            enabled_count = bin(self.register_map.read_reg(FPGARegisters.ENABLE_REG)).count('1')
            self.register_map.write_reg(addr, enabled_count)
        
        value = self.register_map.read_reg(addr)
        logging.info(f"Register read - Addr: 0x{addr:02x}, Value: 0x{value:08x}")
        return value

    def stop(self):
        """Stop the FPGA simulation server"""
        self.running = False
        self.server.close()
        if os.path.exists(self.socket_path):
            os.unlink(self.socket_path)

    def start(self):
        """Start the FPGA simulation server"""
        # Remove existing socket if present
        if os.path.exists(self.socket_path):
            os.unlink(self.socket_path)

        # Create Unix domain socket
        self.server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.server.bind(self.socket_path)
        self.server.listen(1)
        
        # Initialize VHDL simulation
        self.vhdl_sim.initialize()
        self.running = True
        
        print("FPGA simulation server started")
        
        while self.running:
            conn, addr = self.server.accept()
            thread = threading.Thread(target=self.handle_connection, args=(conn,))
            thread.start()

if __name__ == "__main__":
    server = FPGAServer()
    try:
        server.start()
    except KeyboardInterrupt:
        print("\nShutting down FPGA simulation server...")
        server.stop()