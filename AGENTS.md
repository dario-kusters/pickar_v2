# AGENTS.md

## Project

This is production firmware for a custom battery/power-control board
based on an AVR64DD14 microcontroller and deployed on working hardware.

Read `README.md` for repository status and `docs/system-overview.md` for
the hardware architecture and implemented operating behavior.

Treat the current behavior as intentional unless there is evidence in the
source code or documentation that it is a bug.

## General rules

- Preserve existing behavior unless the requested task explicitly requires a change.

- Keep changes focused on the requested task, but do not avoid refactoring
  or restructuring when it clearly improves correctness, reliability,
  maintainability or clarity.

- Avoid unrelated changes that provide no meaningful benefit.

- Do not reorganize files or rename functions merely for style.

- Preserve direct low-level AVR peripheral control where it is appropriate.
  Do not introduce additional frameworks or abstraction layers unless they
  provide a clear technical benefit.

- Do not change MCU configuration bits, clock configuration, interrupt setup,
  GPIO polarity, ADC scaling, timer configuration or peripheral initialization
  unless explicitly required and justified.

- Battery-voltage thresholds, current-sensing thresholds, timeouts and
  power-control parameters may be changed when requested.
  When changing them, identify their physical meaning, units and any
  relationships with ADC scaling, hardware circuitry or other thresholds
  before implementing the change.

- Preserve useful existing comments.

- Distinguish clearly between facts found in the source code and assumptions.

- The documentation is incomplete. If documentation and implementation differ,
  report the difference instead of silently changing either one.

## Before modifying firmware

First inspect the complete project and determine:

- project structure
- compiler/toolchain
- MCU configuration
- clock configuration
- program entry point
- interrupt service routines
- timers
- ADC usage
- GPIO assignments
- button handling
- LED and buzzer control
- SMPS control
- voltage measurement
- current measurement
- power-on and power-off state logic
- battery-low logic and shutdown timing

For significant changes, explain the reasoning and the resulting behavior.
When several technically reasonable implementations exist, describe the
important trade-offs.

## Verification

After modifications:

- review the complete diff
- look for unintended behavioral changes
- check for integer overflow, signed/unsigned issues and timing changes
- check interrupt/shared-variable interactions
- build the original MPLAB X project if the required toolchain is available
- do not modify project/toolchain configuration merely to make a build succeed
