# PROJECT_CONTEXT.md

## Project overview

This project is the firmware for a custom battery-management and power-control
system installed in six warehouse carts.

The complete system was designed and built as a custom project, including the
BMS PCB and the AVR64DD14 firmware.

The firmware was developed using MPLAB X IDE.

The available project documentation was never fully completed, but it provides
a faithful high-level description of the implemented system.

## Purpose of the system

The warehouse carts are used by operators to collect items while viewing work
orders on a computer.

Depending on the cart, the computer is either:

* a laptop
* an Intel NUC mini-PC with a touchscreen

The power system is intended to supply the computer for a complete working
shift of approximately 9 to 10 hours.

At the end of the working day, the carts are connected to battery chargers
until the following shift.

## Battery pack

The system uses four lead-acid batteries.

Each battery is:

* 12 V
* 9 Ah
* intended for cycle use

The four batteries are connected in a series-parallel configuration.

Resulting nominal battery pack:

* Voltage: 24 V
* Capacity: 18 Ah
* Nominal stored energy: 432 Wh

The battery charger used with the system provides approximately 27.6 V for
charging the complete battery pack.

## SMPS module

The computer is powered through an external non-isolated buck/boost SMPS
module.

According to the original project documentation:

* SMPS input comes from the 24 V battery pack
* output voltage is adjusted to 19 V DC
* output current is limited to approximately 4 A
* the module can be switched on and off through a digital control input
  called `SMPS.Ctrl`

The manufacturer specifies an efficiency of approximately 94%.

## BMS module

The BMS is a custom-designed PCB based on an AVR64DD14 microcontroller.

The BMS performs the following functions:

* measures the voltage at the 24 V battery / SMPS input
* measures the current supplied at the SMPS output
* controls a red/green bicolor LED
* controls a buzzer
* reads a user push button
* controls the SMPS through an open-collector digital output

The LED indicates the approximate battery state:

* green when the batteries are charged
* progressively changing toward red as the batteries discharge

## Power-on behavior

With charged batteries, the operator presses the button to enable the SMPS.

The SMPS is initially enabled for approximately one minute.

During this period, the operator is expected to switch on the computer.

If the BMS detects a load current greater than approximately 30 mA, the
one-minute countdown is aborted and the system continues operating normally.

If sufficient load current is not detected during this startup period, the
SMPS is switched off.

## Automatic power-off behavior

During normal operation, the BMS monitors the SMPS load current.

When the measured load current falls below approximately 30 mA, the firmware
assumes that the PC and monitor have been switched off.

The BMS then disables the SMPS.

## Manual power-off

The operator can manually switch off the SMPS by holding the push button for
more than approximately 6 seconds.

## Low-battery behavior

The BMS continuously monitors battery condition.

According to the original project documentation, when the battery state
reaches approximately 20% remaining charge:

1. the buzzer begins producing an acoustic warning
2. the warning continues for approximately 10 minutes
3. after this period the BMS disables the SMPS regardless of the load current

## Important note about this document

This file provides system-level context for understanding the firmware.

It should not be treated as a complete firmware specification.

The original project documentation was incomplete, and implementation details
must be determined from the actual source code.

In particular, values such as:

* voltage thresholds
* ADC values
* current thresholds
* timing constants
* GPIO assignments
* peripheral configuration
* internal states and control logic

must be obtained from the firmware itself.

If the source code differs from this document, report the discrepancy rather
than assuming automatically that either the documentation or the firmware is
correct.
