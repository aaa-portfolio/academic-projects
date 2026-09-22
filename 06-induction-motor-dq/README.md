# Induction Motor Startup Transient — dq Synchronous Frame Modeling

Coursework assignment (co-authored derivation/simulation).

## Overview
Modeled and simulated the super-synchronous startup transient of a 30kW induction motor in the dq synchronous reference frame, characterizing regenerative torque behavior during grid connection.

## Key Results
- Derived and implemented the dq-frame state equations (stator currents, rotor flux linkages, electromagnetic torque) for a squirrel-cage induction motor in MATLAB/Simulink
- Simulated stator connection at 120% synchronous speed, showing the machine operates as a generator, with negative torque decelerating it back to synchronous speed
- Verified the dq-frame model's key advantage — DC-valued steady-state currents — matching analytically predicted settling values (Ids → −26A, Iqs → 0)

## Tools
MATLAB/Simulink

## Files
- `report.pdf` — assignment report with full derivation and results
