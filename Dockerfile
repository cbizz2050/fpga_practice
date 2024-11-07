# Use Ubuntu as base image
FROM ubuntu:22.04

# Avoid timezone prompt during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    ghdl \
    make \
    && rm -rf /var/lib/apt/lists/*

# Create working directory
WORKDIR /app

# Create directory structure
RUN mkdir -p \
    counter_vhdl \
    fpga_power_util_c/src \
    fpga_power_util_c/include \
    fpga_power_util_c/build/vhdl_work \
    && chmod -R 777 /app

# Copy VHDL files
COPY counter_vhdl/*.vhd /app/counter_vhdl/

# Copy C application files
COPY fpga_power_util_c/src/ /app/fpga_power_util_c/src/
COPY fpga_power_util_c/include/fpga_api.h /app/fpga_power_util_c/include/
COPY fpga_power_util_c/Makefile /app/fpga_power_util_c/

# Set permissions for all files and directories
RUN chmod -R 755 /app/counter_vhdl && \
    chmod -R 755 /app/fpga_power_util_c && \
    chmod 644 /app/fpga_power_util_c/include/*.h && \
    chmod 644 /app/fpga_power_util_c/src/*.c && \
    chmod 644 /app/fpga_power_util_c/Makefile && \
    chmod -R 755 /app/fpga_power_util_c/build

# Set working directory for compilation
WORKDIR /app/fpga_power_util_c

# Create GHDL work directory
RUN mkdir -p build/vhdl_work && \
    ghdl --clean && \
    chmod -R 777 build

# Build VHDL files
RUN make vhdl

RUN make c_app


RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN mkdir fpga_sim

COPY fpga_sim/ /app/fpga_sim/

# Set the default command
CMD ["/bin/bash"]