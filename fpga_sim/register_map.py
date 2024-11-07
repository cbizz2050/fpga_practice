from enum import IntEnum

class FPGARegisters(IntEnum):
    RESET_REG   = 0x00
    ENABLE_REG  = 0x04
    STATUS_REG  = 0x08
    CONFIG_REG  = 0x0C

class RegisterMap:
    def __init__(self):
        self.registers = {
            FPGARegisters.RESET_REG: 0,
            FPGARegisters.ENABLE_REG: 0,
            FPGARegisters.STATUS_REG: 0,
            FPGARegisters.CONFIG_REG: 256  # MAX_COUNTERS
        }
        self.state_file = "fpga_state.dat"
        self._load_state()

    def _save_state(self):
        with open(self.state_file, 'w') as f:
            for addr, value in self.registers.items():
                f.write(f"{addr}:{value}\n")

    def _load_state(self):
        try:
            with open(self.state_file, 'r') as f:
                for line in f:
                    addr, value = map(int, line.strip().split(':'))
                    self.registers[addr] = value
        except FileNotFoundError:
            self._save_state()

    def write_reg(self, addr, data):
        self.registers[addr] = data
        self._save_state()

    def read_reg(self, addr):
        return self.registers.get(addr, 0)