# NEORV32 - FPHUB Research Implementation

> **⚠️ RESEARCH FORK WARNING** > This is a **modified fork** of the official [NEORV32 Processor](https://github.com/stnolting/neorv32).  
> If you are looking for the original, stable RISC-V processor, please visit the [upstream repository](https://github.com/stnolting/neorv32).

## About this Project

This repository contains a research implementation of the **FPHUB Floating-Point Format** integrated into the NEORV32 architecture. This work is part of a research project at the **University of Málaga (UMA)** aimed at exploring simplified floating-point arithmetic units for reduced hardware complexity.

## What is FPHUB?

Unlike the IEEE 754 standard, this unit implements the **FPHUBv2** format, which simplifies hardware by redefining the exponent representation and special cases:

* **Bias:** Represented in excess $2^{n_{exp}-1}$ (instead of the standard $2^{n_{exp}-1}-1$).
* **No Subnormals:** Subnormal numbers are not considered in order to simplify hardware.
* **Special Cases:** Defined with specific hardware-friendly codes:
    * **Zero (0):** Exponent and significand fields are "all 0s".
    * **One (1):** Significand is "all 0s" and the exponent is equal to the bias ($2^{n_{exp}-1}$).
    * **Infinity ($\infty$):** Exponent and significand fields are "all 1s".
* **Truncation:** Rounding modes are simplified to truncation (Round towards Zero).

For more information, you can visit our [organization](https://github.com/HUBformat).

## Modifications & Status

The modifications are concentrated in the Floating-Point Unit (FPU) VHDL source files [rtl/core/neorv32_cpu_cp_fpu.vhd](rtl/core/neorv32_cpu_cp_fpu.vhd).

### Implemented Features
The following operations have been adapted ("hubbed") to the FPHUB format and verified:
* **FADD / FSUB:** Single-precision addition and subtraction with Bias 128.
* **FMUL:** Multiplication logic adapted for the new exponent range.
* **FCVT (I2F / F2I):** Integer-Float conversions adjusted for the new bias and truncation rules.
* **FCOMP:** Comparisons (FEQ, FLT, FLE) adapted to the format.

## Repository Structure
* `rtl/core/neorv32_cpu_cp_fpu.vhd`: The core FPU logic modified for FPHUB.
* `sw/example/`: Custom test benches used to validate the FPHUB arithmetic correctness.

![Logos of the Spanish Government, European Union NextGenerationEU,
Spanish Recovery and Resilience Plans, and Spanish State Research
Agency.](https://github.com/HUBformat/.github/raw/main/profile/res/MICIU+NextG+PRTR+AEI.svg)&nbsp;