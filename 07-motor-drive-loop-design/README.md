# DC & Induction Motor Drive — Current/Speed Loop Design and Flux-Control Comparison

Individual coursework assignment.

## Overview
Designed current and speed control loops for a DC motor drive using symmetrical-optimum tuning, and compared flux-control architectures for an induction motor drive.

## Key Results
- Designed a current control loop (500Hz bandwidth) for a full-bridge-converter-fed DC motor, incorporating PWM transport delay, then analytically derived the oscillation frequency that emerges when bandwidth is increased to 2.5kHz
- Designed DC motor speed control loops via the symmetrical optimum method under two design targets — standard 4τ and 55° phase margin — comparing disturbance rejection performance
- Compared single-loop vs. two-loop rotor flux control architectures for an induction machine, evaluating steady-state error, transient response, and stability margins

## Tools
MATLAB/Simulink

## Files
- `report.pdf` — assignment specification and problem statement
