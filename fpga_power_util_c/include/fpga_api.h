#ifndef FPGA_API_H
#define FPGA_API_H

#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// FPGA Register interface definitions
typedef uint32_t fpga_addr_t;   // Type for FPGA register addresses
typedef uint32_t fpga_data_t;   // Type for FPGA register data

// Socket communication definitions
#define FPGA_SIM_SOCKET "/tmp/fpga_sim.sock"
#define CMD_READ  0
#define CMD_WRITE 1

/**
 * @brief Write data to an FPGA register through simulator
 * @param addr Register address
 * @param data Data to write
 * @return void, prints error message on failure
 */
void fpga_write_reg(fpga_addr_t addr, fpga_data_t data);

/**
 * @brief Read data from an FPGA register through simulator
 * @param addr Register address
 * @return Data read from register, 0 on failure
 */
fpga_data_t fpga_read_reg(fpga_addr_t addr);

/**
 * @brief Reset the FPGA to its initial state
 * @param reset_reg Address of reset control register
 * @details Writes 1 then 0 to reset register
 */
void reset_fpga(fpga_addr_t reset_reg);

/**
 * @brief Enable a specified number of counters
 * @param enable_reg Address of counter enable register
 * @param num_counters Number of counters to enable
 * @details Creates and writes enable mask based on counter count
 */
void enable_counters(fpga_addr_t enable_reg, uint32_t num_counters);

/**
 * @brief Get the current number of enabled counters
 * @param status_reg Address of counter status register
 * @return Number of currently enabled counters
 * @details Counts number of set bits in status register
 */
uint32_t get_num_counters_enabled(fpga_addr_t status_reg);

/**
 * @brief Get the maximum number of counters that can be enabled
 * @param config_reg Address of configuration register
 * @return Maximum number of available counters
 * @details Reads directly from configuration register
 */
uint32_t get_max_num_counters(fpga_addr_t config_reg);

#endif /* FPGA_API_H */