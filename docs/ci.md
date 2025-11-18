# Continuous Integration (CI) Workflow

This document explains the CI setup for the project.

---

## Overview

Every push and pull request triggers:
- Multi‑OS build matrix  
- Python setup  
- Strict firmware compilation  
- Simulation run  
- Artifact upload  

---

## Build Matrix

| OS              | Python Versions |
|-----------------|-----------------|
| Ubuntu‑latest   | 3.10, 3.11      |
| macOS‑latest    | 3.10, 3.11      |
| Windows‑latest  | 3.10, 3.11      |

A total of **6 jobs per workflow**.

---

## Workflow Steps

1. Check out repository  
2. Install Python + deps  
3. Verify gcc  
4. Run simulation script  
5. Upload artifacts:  
   - `log.csv`  
   - `step_response.png`  

---

## Artifact Inspection

1. Open the **Actions** tab  
2. Select a workflow run  
3. Download artifacts named:  
   - `pid-simulation-<os>-py<version>`  
4. Extract to view logs and plots  

---

## Why It Matters

- Ensures code quality  
- Prevents regressions  
- Demonstrates professional build/testing practices  
- Provides reproducible simulation results across platforms  

---

## Future CI Improvements

- Unit test execution  
- Gain sweep automation  
- Pages deployment with plots  
