#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include "fpga_api.h"

#define FPGA_SIM_SOCKET "/tmp/fpga_sim.sock"
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[FPGA Client] " fmt "\n", ##__VA_ARGS__)

// Command definitions for socket communication
#define CMD_READ  0
#define CMD_WRITE 1


static int connect_to_simulator(void) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        DEBUG_PRINT("Socket creation failed");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, FPGA_SIM_SOCKET, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        DEBUG_PRINT("Connection failed");
        close(fd);
        return -1;
    }

    DEBUG_PRINT("Connected to FPGA simulator");
    return fd;
}

static void send_command(int fd, uint8_t cmd, fpga_addr_t addr, fpga_data_t data) {
    uint8_t buffer[9];  // 1 byte cmd + 4 bytes addr + 4 bytes data
    buffer[0] = cmd;
    // Network byte order for address
    buffer[1] = (addr >> 24) & 0xFF;
    buffer[2] = (addr >> 16) & 0xFF;
    buffer[3] = (addr >> 8) & 0xFF;
    buffer[4] = addr & 0xFF;
    // Network byte order for data
    buffer[5] = (data >> 24) & 0xFF;
    buffer[6] = (data >> 16) & 0xFF;
    buffer[7] = (data >> 8) & 0xFF;
    buffer[8] = data & 0xFF;

    write(fd, buffer, cmd == CMD_WRITE ? 9 : 5);
}

void fpga_write_reg(fpga_addr_t addr, fpga_data_t data) {
    DEBUG_PRINT("Writing to register 0x%02x: 0x%08x", addr, data);
    
    int fd = connect_to_simulator();
    if (fd < 0) return;

    uint8_t buffer[9];
    buffer[0] = CMD_WRITE;
    buffer[1] = (addr >> 24) & 0xFF;
    buffer[2] = (addr >> 16) & 0xFF;
    buffer[3] = (addr >> 8) & 0xFF;
    buffer[4] = addr & 0xFF;
    buffer[5] = (data >> 24) & 0xFF;
    buffer[6] = (data >> 16) & 0xFF;
    buffer[7] = (data >> 8) & 0xFF;
    buffer[8] = data & 0xFF;

    write(fd, buffer, 9);
    
    uint8_t response;
    read(fd, &response, 1);
    DEBUG_PRINT("Write operation completed with status: %d", response);
    
    close(fd);
}

fpga_data_t fpga_read_reg(fpga_addr_t addr) {
    DEBUG_PRINT("Reading from register 0x%02x", addr);
    
    int fd = connect_to_simulator();
    if (fd < 0) return 0;

    uint8_t buffer[5];
    buffer[0] = CMD_READ;
    buffer[1] = (addr >> 24) & 0xFF;
    buffer[2] = (addr >> 16) & 0xFF;
    buffer[3] = (addr >> 8) & 0xFF;
    buffer[4] = addr & 0xFF;

    write(fd, buffer, 5);
    
    uint8_t read_buffer[4];
    read(fd, read_buffer, 4);
    fpga_data_t value = (read_buffer[0] << 24) | (read_buffer[1] << 16) |
                        (read_buffer[2] << 8) | read_buffer[3];
    
    DEBUG_PRINT("Read value: 0x%08x", value);
    
    close(fd);
    return value;
}

void reset_fpga(fpga_addr_t reset_reg) {
    fpga_write_reg(reset_reg, 1);
    fpga_write_reg(reset_reg, 0);
}

void enable_counters(fpga_addr_t enable_reg, uint32_t num_counters) {
    // Create proper enable mask where each bit represents one counter
    fpga_data_t enable_mask = 0;
    for (uint32_t i = 0; i < num_counters && i < 32; i++) {
        enable_mask |= (1U << i);
    }
    fpga_write_reg(enable_reg, enable_mask);
}

uint32_t get_num_counters_enabled(fpga_addr_t status_reg) {
    // Read directly from status register which contains count of enabled counters
    return fpga_read_reg(status_reg);
}

uint32_t get_max_num_counters(fpga_addr_t config_reg) {
    return fpga_read_reg(config_reg);
}