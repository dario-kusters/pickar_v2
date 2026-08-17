# pickar_v2

Firmware for a custom battery and power-management controller based on the **Microchip AVR64DD14**.

The controller is installed in portable warehouse carts used to power a laptop or an Intel NUC with touchscreen during a complete working shift.

The complete system, including PCB hardware and firmware, was developed as a custom project.

## System overview

The power system consists of:

- Four 12 V / 9 Ah lead-acid batteries
- Series-parallel battery configuration: **24 V / 18 Ah nominal**
- External non-isolated buck/boost SMPS
- SMPS output: approximately **19 V**
- Custom AVR64DD14-based BMS / power-control PCB

The AVR64DD14 controller remains permanently powered from the battery pack.

The external SMPS is switched on and off by the BMS and supplies the PC and monitor.

## Main BMS functions

The firmware:

- measures battery voltage
- measures SMPS load current
- controls the SMPS enable signal
- monitors PC/monitor power consumption
- controls a red/green bicolor status LED
- controls an acoustic buzzer
- reads the operator push button
- manages startup and shutdown sequences
- performs automatic shutdown when the PC/monitor is no longer consuming significant current
- provides low-battery warning and shutdown behavior

## Normal operating sequence

The principal operating cycle is:

```text
BMS_IDLE
   |
   | button
   v
SMPS_ON
   |
   | PC load detected
   v
PC_ON
   |
   | PC shutdown / manual shutdown / battery shutdown
   v
SMPS_OFF
   |
   | button released
   v
BMS_IDLE
```

The AVR itself is normally not reset between these cycles. The sequence may therefore be repeated hundreds or thousands of times during the lifetime of the system.

## Load-current detection

The current measurement monitors the load connected to the **output of the SMPS**.

It does not include the current consumption of the BMS itself.

The no-load threshold is intentionally approximately:

```text
23.2 mA
```

This value allows the firmware to recognize the condition in which the PC has been switched off while the monitor or other connected circuitry may still consume a small standby current.

This threshold is intentional and should not be changed without reviewing the actual system behavior.

## ADC channel sequencing

The ADC operates in free-running mode and alternates between the voltage and current measurement channels.

The apparent relationship between ADC6 / ADC7 and the application variables may look reversed when reading the code.

This behavior is intentional and is related to the ADC multiplexer/conversion sequencing in free-running operation.

**Do not change the ADC6/ADC7 sequencing merely because it appears reversed in the source code.**

## Battery voltage thresholds

The firmware contains several battery-voltage thresholds used for:

- LED indication
- low-battery warnings
- battery-state substates
- shutdown timing

These values are currently functional but are not considered precisely calibrated.

They have intentionally **not been modified during the current firmware maintenance work**.

Battery-voltage threshold calibration is deferred to a later revision.

## Historical battery-charging functionality

The original design included concepts for:

```text
Batt_LOW
    -> Batt_Charging
    -> Batt_FULL
```

together with battery-cycle and EEPROM-related functionality.

This part of the original design was never completed.

The corresponding historical code and states may still be visible in the firmware, but the unfinished charging path is intentionally kept inaccessible.

It must not be reactivated without reviewing and completing the entire associated design.

## Historical state diagram

The repository contains the original state-machine design diagram:

```text
docs/pickar_v2_State_diagram.wsd
```

This file documents the original design intent and is useful when interpreting the firmware.

However, it is historical documentation and does not necessarily represent the final implemented behavior.

The actual source code remains authoritative for determining current firmware behavior.

## Firmware maintenance

The firmware was originally developed several years ago under significant time constraints.

A recent review focused on improving reliability and the behavior of repeated ON/OFF sessions without redesigning the system.

Areas reviewed include:

- main state-machine transitions
- substate initialization
- state context preserved between sessions
- SMPS startup/shutdown behavior
- button handling
- current-detection timing
- low-battery state initialization
- UART command handling
- unused timer configuration

The intention of this maintenance work is to correct identifiable implementation defects while preserving the established behavior of the deployed system.

## Development UART

USART0 is provided primarily as a development and diagnostic interface.

It is normally **not connected or used in deployed units**.

A historical RX-buffer/parser problem was identified during firmware review and corrected.

The RX input uses an internal pull-up so that the input has a defined idle level when no external UART is connected.

## Toolchain

Current project configuration:

- MCU: **AVR64DD14**
- MPLAB X IDE: **6.30**
- XC8 compiler: **4.00**
- AVR-Dx DFP: **2.8.343**

The project currently builds successfully with this toolchain.

## Repository structure

Main files and directories include:

```text
pickar_v2/
├── AGENTS.md
├── PROJECT_CONTEXT.md
├── README.md
├── BMS_V1.mc3
├── main.c
├── include/
├── mcc_generated_files/
├── nbproject/
└── docs/
    └── pickar_v2_State_diagram.wsd
```

### `main.c`

Contains the application logic, including the principal state machine, battery substates, timing logic, LED/buzzer behavior and development command interface.

### `include/`

Contains application-level declarations, state definitions, flags and global definitions.

### `mcc_generated_files/`

Peripheral initialization and drivers generated with Microchip Code Configurator.

Generated code should not be modified casually. Where possible, configuration changes should be made through MCC so that they survive future regeneration.

### `BMS_V1.mc3`

MCC project configuration.

### `PROJECT_CONTEXT.md`

High-level description of the physical system and intended behavior.

### `AGENTS.md`

Engineering instructions and constraints for AI-assisted work on the repository.

## Current status

The current firmware:

- builds successfully with MPLAB X 6.30 / XC8 4.00
- retains the original battery-voltage thresholds
- retains the intentional ~23.2 mA no-load threshold
- retains the existing ADC free-running channel sequencing
- does not activate the unfinished battery-charging functionality
- includes recent state-machine and reliability corrections

**The latest firmware changes still require validation on the real BMS hardware before being considered production-tested.**

The next immediate step is therefore to program a BMS module with the generated firmware and verify normal startup, operation and shutdown behavior on the actual system.