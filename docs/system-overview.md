# System overview

## Scope and source of truth

This document describes the physical system and the behavior implemented by
the firmware. It is not a complete firmware specification.

Some hardware data comes from the original project documentation and is marked
as nominal or approximate. Firmware behavior and implementation values have
been checked against the source code. If this document and the implementation
differ, the source code is authoritative and the discrepancy should be
reported.

## Hardware architecture

The system powers a laptop or an Intel NUC with touchscreen on a portable
warehouse cart. Its main components are:

- four 12 V / 9 Ah cycle-use lead-acid batteries in a series-parallel
  configuration, giving 24 V / 18 Ah and 432 Wh nominal
- a charger providing approximately 27.6 V to the complete battery pack
- an external non-isolated buck/boost SMPS, nominally adjusted to 19 V with an
  approximately 4 A output-current limit
- a custom power-control PCB based on an AVR64DD14 microcontroller

The original SMPS documentation specifies approximately 94% efficiency. The
SMPS is powered from the battery pack and is enabled by the controller through
an open-collector control output. Its output supplies the computer and display.

The AVR64DD14 remains powered while the SMPS is off. The controller:

- measures battery voltage and SMPS output current
- controls the SMPS, a red/green bicolor LED and a buzzer
- reads the operator push button
- manages startup, normal operation and shutdown

## Operating sequence

The normal state sequence is:

```text
BMS_IDLE
   |
   | button pressed
   v
SMPS_ON
   |
   | PC load detected
   v
PC_ON
   |
   | no load, long button press or low battery
   v
SMPS_OFF
   |
   | button released
   v
BMS_IDLE
```

The AVR is normally not reset between operating cycles, so state belonging to
one session must not leak into the next.

### Startup

Pressing the button in `BMS_IDLE` enables the SMPS for up to 60 seconds. Near
the start of this interval, the controller briefly drives the PC power-switch
output.

The firmware enters `PC_ON` when the averaged current ADC value exceeds 125.
With the conversion used by the diagnostic output, this represents
approximately 100 mA. If that load is not detected before the timeout, the SMPS
is disabled and the controller returns to `BMS_IDLE`.

### Automatic and manual shutdown

Current is measured at the SMPS output and therefore excludes the BMS's own
consumption. During normal operation, the no-load threshold defaults to ADC
value 29, approximately 23.2 mA. Three consecutive one-second averages at or
below this threshold trigger the shutdown path. This deliberately permits a
small amount of standby consumption and should not be changed without testing
the complete system.

Holding the operator button for more than five one-second checks, approximately
six seconds, switches the SMPS off manually.

### Low-battery behavior

The firmware uses these battery-voltage thresholds:

| Indicated level | ADC value | Approximate pack voltage |
| --- | ---: | ---: |
| 80% | 3260 | 26.08 V |
| 30% | 2785 | 22.28 V |
| 20% | 2690 | 21.52 V |

These inherited percentage labels and voltages are used for LED indication,
warnings and shutdown states, but they have not been precisely calibrated as a
state-of-charge model.

Below the 20% threshold, the firmware warns for 600 seconds and then enters the
PC shutdown sequence. It drives the PC power-switch output for approximately
five seconds and waits for the load to disappear, with a maximum shutdown
timer of 120 seconds, before disabling the SMPS.

## ADC channel sequencing

The ADC runs in free-running mode and alternates between the voltage and current
channels. The ADC6/ADC7 relationship to the application variables can appear
reversed because the result is handled together with multiplexer advancement.
This sequencing is intentional and must not be swapped solely because it looks
reversed in the application code.

## Historical charging states

The original design contains `Batt_LOW`, `Batt_Charging` and `Batt_FULL` states,
as well as unfinished battery-cycle and EEPROM concepts. They do not form a
completed, validated charging feature and must not be enabled or extended
without reviewing the complete design.

The historical state diagram is stored in
`docs/pickar_v2_State_diagram.wsd`. It records design intent and may differ from
the implemented state machine.

## Development UART

USART0 is a development and diagnostic interface and is normally not connected
on deployed units. Its RX input uses the internal pull-up so it has a defined
idle level when no external UART is attached.
