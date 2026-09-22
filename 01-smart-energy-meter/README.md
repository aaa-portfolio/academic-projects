# Smart Energy Meter — Real-Time Harmonic Analysis

TI C2000 F280049C coursework project.
## Overview
Built and validated a real-time power quality monitor implementing Goertzel DFT-based harmonic extraction, computing THD, true power factor, and active/reactive power.

## Key Results
- Signal acquisition via dual ADC channels, synchronized through ePWM at 6400Hz, resolving harmonics up to the 15th order
- Validated harmonic accuracy against a known square-wave test signal
- Computed true power factor via PF = DF × (I₁/I_RMS)

## Tools
TI C2000 (F280049C), Embedded C

## Files
- `src/` — C source files
