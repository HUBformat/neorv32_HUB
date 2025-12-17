# Application makefile

# Override the default CPU ISA
MARCH = rv32i_zicsr_zifencei_zfinx

# Override default optimization goal
EFFORT = -Os

# Override the default RISC-V GCC prefix
#RISCV_PREFIX ?= riscv-none-elf-

# Adjust processor IMEM size (increase for C++ and float math)
USER_FLAGS += -Wl,--defsym,__neorv32_rom_size=64k

# Adjust processor DMEM size
USER_FLAGS += -Wl,--defsym,__neorv32_ram_size=16k

# Set path to NEORV32 root directory
NEORV32_HOME ?= ../../..

# Include the main NEORV32 makefile
include $(NEORV32_HOME)/sw/common/common.mk
