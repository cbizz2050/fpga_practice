#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "fpga_api.h"

// FPGA Register Addresses (must match fpga_sim's register_map.py)
#define FPGA_RESET_REG   0x00
#define FPGA_ENABLE_REG  0x04
#define FPGA_STATUS_REG  0x08
#define FPGA_CONFIG_REG  0x0C

#define POWER_PER_COUNTER 10  // Assuming each counter consumes 10mW

// Function prototypes
static void print_current_power(void);
static uint32_t power_to_counters(uint32_t power_mw);
static uint32_t counters_to_power(uint32_t num_counters);
static void print_usage(const char* program_name);

static void print_current_power(void) {
    uint32_t current_counters = get_num_counters_enabled(FPGA_STATUS_REG);
    uint32_t current_power = counters_to_power(current_counters);
    printf("Current power consumption is %u mW\n", current_power);
}

static uint32_t power_to_counters(uint32_t power_mw) {
    return (power_mw + POWER_PER_COUNTER - 1) / POWER_PER_COUNTER;
}

static uint32_t counters_to_power(uint32_t num_counters) {
    return num_counters * POWER_PER_COUNTER;
}

static void print_usage(const char* program_name) {
    printf("Usage:\n");
    printf("  Set power consumption:\n");
    printf("    %s <power_in_mw>\n", program_name);
    printf("  Print current power consumption:\n");
    printf("    %s\n", program_name);
    printf("\nExample:\n");
    printf("  %s 500    # Set power consumption to 500mW\n", program_name);
    printf("  %s        # Print current power consumption\n", program_name);
}

int main(int argc, char *argv[]) {
    // If no arguments, just print current power consumption
    if (argc == 1) {
        print_current_power();
        return 0;
    }
    
    // If one argument provided, set the power consumption
    if (argc == 2) {
        // Check if the argument is "help" or "-h"
        if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        
        // Convert argument to integer
        char *endptr;
        uint32_t requested_power = strtoul(argv[1], &endptr, 10);
        
        // Validate conversion
        if (*endptr != '\0') {
            fprintf(stderr, "Error: Invalid power value\n");
            print_usage(argv[0]);
            return 1;
        }
        
        // Calculate required number of counters
        uint32_t required_counters = power_to_counters(requested_power);
        uint32_t max_counters = get_max_num_counters(FPGA_CONFIG_REG);
        
        // Validate against maximum power
        if (required_counters > max_counters) {
            fprintf(stderr, "Error: Requested power (%u mW) exceeds maximum possible power (%u mW)\n",
                    requested_power, counters_to_power(max_counters));
            return 1;
        }
        
        // Reset FPGA and enable required number of counters
        reset_fpga(FPGA_RESET_REG);
        enable_counters(FPGA_ENABLE_REG, required_counters);
        
        // Print the current power consumption after setting
        print_current_power();
        return 0;
    }
    
    // If more than one argument provided, show usage
    fprintf(stderr, "Error: Too many arguments\n");
    print_usage(argv[0]);
    return 1;
}