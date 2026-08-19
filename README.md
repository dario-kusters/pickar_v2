# pickar_v2

Firmware for a custom battery and power-management controller based on the
**Microchip AVR64DD14**. The controller manages the 19 V supply for a computer
and display on a battery-powered warehouse cart.

This is an existing embedded system deployed on working hardware. Maintenance
focuses on correcting identifiable defects while preserving established
behavior.

## Main functions

- battery-voltage and SMPS load-current measurement
- SMPS enable and PC power-switch control
- operator push-button handling
- red/green status LED and acoustic warnings
- startup, automatic shutdown, manual shutdown and low-battery sequencing

## Documentation

- [`docs/system-overview.md`](docs/system-overview.md) describes the hardware,
  operating sequence, measurement thresholds and historical design elements.
- [`docs/pickar_v2_State_diagram.wsd`](docs/pickar_v2_State_diagram.wsd) is the
  original state-machine diagram. It records historical design intent and is
  not authoritative where it differs from the firmware.
- [`AGENTS.md`](AGENTS.md) contains the engineering constraints for
  AI-assisted maintenance.

The source code is authoritative for implemented behavior. Read the system
overview and the repository instructions before modifying the firmware.

## Toolchain

The checked project configuration is:

- MCU: **AVR64DD14**
- MPLAB X IDE: **6.30**
- XC8 compiler: **4.00**
- AVR-Dx DFP: **2.8.343**

The project builds successfully with this toolchain. Do not change project or
toolchain configuration merely to make a build succeed with a different local
setup.

## Repository layout

- `main.c`: application state machine, timing, measurements, user interface and
  diagnostic command handling
- `include/`: application declarations, state definitions and flags
- `mcc_generated_files/`: MCC-generated peripheral initialization and drivers
- `BMS_V1.mc3`: MCC project configuration
- `nbproject/`: MPLAB X project files
- `docs/`: system documentation and historical design material

Generated MCC code should not be edited casually. When practical, peripheral
configuration changes should be made through MCC so they survive regeneration.

## Current status

Recent maintenance addressed state-machine behavior across repeated ON/OFF
sessions, UART input handling and related reliability issues. These changes
build successfully but still require validation on the real BMS hardware before
being considered production-tested.
