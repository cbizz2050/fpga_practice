CC = gcc
VHDL = ghdl
CFLAGS = -Wall -Wextra -I./include
BUILD_DIR = build
VHDL_DIR = ../counter_vhdl
WORK_DIR = $(BUILD_DIR)/vhdl_work

# Source files
SRCS = src/main.c src/fpga_api.c
OBJS = $(SRCS:src/%.c=$(BUILD_DIR)/%.o)
VHDL_SRCS = $(VHDL_DIR)/counter_load.vhd $(VHDL_DIR)/up_counter.vhd

# Target executable
TARGET = $(BUILD_DIR)/fpga_power_util

.PHONY: all clean vhdl c_app

all: $(BUILD_DIR) vhdl c_app

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/src
	mkdir -p $(WORK_DIR)

# Compile VHDL files
vhdl: $(BUILD_DIR)
	$(VHDL) --clean
	$(VHDL) -a --std=08 --work=work -P$(WORK_DIR) $(VHDL_DIR)/up_counter.vhd
	$(VHDL) -a --std=08 --work=work -P$(WORK_DIR) $(VHDL_DIR)/counter_load.vhd
	$(VHDL) -e --std=08 --work=work -P$(WORK_DIR) counter_load

c_app: $(BUILD_DIR) $(TARGET)

# Compile C source files
$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@


# Link the C application
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	$(VHDL) --clean