# Closed-Loop Boost Converter — Design & Firmware

TI C2000 F280049C coursework project.

## Overview
Designed a boost converter PCB and closed-loop PI control firmware on TI C2000, including component selection, small-signal analysis, and Tustin discretization.

## Key Results
- Designed converter components and PCB for a 48–62V to 64–85V, 200W boost converter at 80kHz switching
- Derived the converter's small-signal model and selected PI gains via loop-shaping for output voltage/inductor current loop control
- Discretized the controller via Tustin's method and implemented it as an ADC-triggered ISR, with ePWM configured for synchronized sampling, on the F280049C

## Tools
TI C2000 (F280049C), Embedded C, KiCAD

## Files
- `src/` — C firmware source files
- `pcb/` — KiCAD schematic/PCB files (if available)
