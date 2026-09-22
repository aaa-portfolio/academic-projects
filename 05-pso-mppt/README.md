# Modified PSO-Based MPPT for PV Systems under Partial Shading

B.Tech Thesis, National Institute of Technology, Warangal.

## Overview
Designed and validated a modified Particle Swarm Optimization (MPSO) MPPT algorithm that reliably tracks the global maximum power point under partial shading conditions, where conventional methods (P&O, standard PSO) fail.

## Key Results
- Designed a modified PSO algorithm — adding search-space reduction, adaptive inertia weighting, and an automatic reset criterion — to overcome local-MPP trapping and slow convergence
- Modeled a 4-module PV array with a DC-DC boost converter and MPPT controller in MATLAB/Simulink, tested across six partial-shading irradiance profiles
- Demonstrated the modified PSO consistently found the true global MPP (>95% efficiency in all 6 test cases), while standard P&O dropped as low as 29.4% efficiency under shading

## Tools
MATLAB/Simulink

## Files
- `MPPT.m` — MPSO MATLAB function
- `thesis_report.pdf` — full B.Tech thesis report
