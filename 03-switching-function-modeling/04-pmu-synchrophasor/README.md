# PMU Synchrophasor Estimation — Sliding & Recursive DFT Under Frequency Step

Python coursework project (group assignment — signal processing and analysis implementation by Abdul Aleem; written report by teammate).

## Overview
Implemented sliding and recursive DFT-based synchrophasor estimators in Python and evaluated their transient accuracy under a step change in system frequency, using Total Vector Error (TVE).

## Key Results
- Built a signal generator with phase-continuous frequency steps and implemented both sliding and recursive DFT phasor estimators
- Measured TVE against the exact synchrophasor value to quantify accuracy following each disturbance
- Discovered that recursive DFT's TVE diverges more than sliding DFT's after the frequency step is removed, despite theoretically identical update equations

## Tools
Python

## Files
- `pmu_synchrophasor.ipynb` — implementation and analysis
