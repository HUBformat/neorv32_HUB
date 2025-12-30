# NEORV32 - FPHUB Research Implementation

> **⚠️ RESEARCH FORK WARNING** > This is a **modified fork** of the official [NEORV32 Processor](https://github.com/stnolting/neorv32).  
> If you are looking for the original, stable RISC-V processor, please visit the [upstream repository](https://github.com/stnolting/neorv32).

## About this Project

This repository contains a research implementation of the **FPHUB Floating-Point Format** integrated into the NEORV32 architecture. This work is part of a research project at the **University of Málaga (UMA)** aimed at exploring simplified floating-point arithmetic units for reduced hardware complexity.

### What is FPHUB?
FPHUB is a non-standard floating-point format designed to reduce silicon area and latency while maintaining precision for specific workloads. Key differences from IEEE 754 implemented in this core:
* **Bias 128:** The exponent bias is shifted to 128 (instead of the standard 127).
* **No Subnormals:** Subnormal numbers are treated as zeros to simplify the datapath.
* **Simplified Handling:** NaNs and Infinities are handled with simplified rules or treated as saturation points within the normal range.
* **Truncation:** Rounding modes are simplified to truncation (Round towards Zero).

For more information, you can visit our [organization](https://github.com/HUBformat).

## Modifications & Status

The modifications are concentrated in the Floating-Point Unit (FPU) VHDL source files [rtl/core/neorv32_cpu_cp_fpu.vhd](rtl/core/neorv32_cpu_cp_fpu.vhd).

### Implemented Features
The following operations have been adapted ("hubbed") to the FPHUB format and verified:
* ✅ **FADD / FSUB:** Single-precision addition and subtraction with Bias 128.
* ✅ **FMUL:** Multiplication logic adapted for the new exponent range.
* ✅ **FCVT (I2F / F2I):** Integer-Float conversions adjusted for the new bias and truncation rules.
* ✅ **FCOMP:** Comparisons (FEQ, FLT, FLE) adapted to the format.

## Repository Structure
* `rtl/core/neorv32_cpu_cp_fpu.vhd`: The core FPU logic modified for FPHUB.
* `sw/example/`: Custom test benches used to validate the FPHUB arithmetic correctness.

![Logos of the Spanish Government, European Union NextGenerationEU,
Spanish Recovery and Resilience Plans, and Spanish State Research
Agency.](https://github.com/HUBformat/.github/raw/main/profile/res/MICIU+NextG+PRTR+AEI.svg)&nbsp;