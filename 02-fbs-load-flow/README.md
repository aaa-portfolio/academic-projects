# FBS Load Flow — Radial Distribution Network

Python coursework project.

## Overview
Implemented a Forward-Backward Sweep (FBS) load flow solver for a 19-node radial distribution network, using tree traversal techniques in an object-oriented framework.

## Key Results
- Modeled the network as a tree of parent-child `Bus` objects, using post-order traversal for the backward sweep (leaf-to-root current summation) and pre-order traversal for the forward sweep (root-to-leaf voltage update)
- Implemented iterative backward-forward sweeps with a convergence check on maximum voltage change
- Computed a full system power flow (11kV, 1000kVA base) with 1.047 MW active and 0.450 Mvar reactive losses, and a maximum voltage deviation of 0.4458 pu at the farthest bus

## Tools
Python

## Files
- `fbs_load_flow.ipynb` — Jupyter notebook with implementation
