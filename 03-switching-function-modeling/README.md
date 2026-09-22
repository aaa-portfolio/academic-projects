# Switching-Function Modeling of Grid-Connected Converters

Python coursework project (2-Level & 5-Level NPC topologies).

## Overview
Derived and simulated switching-function-based converter models to analyze harmonic performance across 2-level and multilevel topologies under grid injection.

## Key Results
- Derived the governing KVL equation for a grid-connected switching pole using a switching-function representation, then extended it to a 5-level NPC topology with phase-disposition PWM
- Built a Python simulation framework (custom PWM, VSC, sliding-window DFT, and ODE-based grid injection classes) to compute pole voltage and injected current waveforms across square-wave, sine-triangle, and sine-sawtooth modulation schemes
- Quantified the harmonic performance improvement from multilevel switching: pole voltage THD dropped from 146.1% (2-level sine-triangle) to 38.3% (5-level NPC phase-disposition) at matched fundamental RMS output

## Tools
Python

## Files
- `two_level.ipynb` — 2-level switching pole modeling
- `five_level_npc.ipynb` — 5-level NPC extension
