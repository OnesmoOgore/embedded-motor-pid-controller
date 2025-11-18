# Embedded Motor PID Controller

Welcome to the documentation for the **Embedded Motor PID Controller** project.

This project demonstrates a portable, embedded C PID speed controller for a DC motor, along with a Python-based simulation and a multi-platform CI pipeline.

---

## 📦 Repository

GitHub repo:  
[https://github.com/OnesmoOgore/embedded-motor-pid-controller](https://github.com/OnesmoOgore/embedded-motor-pid-controller)

---

## 📚 Documentation

- [Architecture Overview](architecture.md)  
  High-level description of the firmware structure, modules, control loop, and simulation environment.

- [CI Workflow Details](ci.md)  
  Details of the GitHub Actions workflows, build matrix, and artifacts.

---

## 🧠 What This Project Demonstrates

- Modular embedded C firmware (separating motor abstraction and PID control logic)  
- A reusable PID module suitable for desktop or MCU builds  
- Python simulation pipeline for step-response analysis and tuning  
- Automated CI on Ubuntu and Windows  
- Strict gcc warning flags and artifact logging (CSV + plots)

---

## 🚀 Getting Started

Refer to the main project README for:

- Build and run instructions  
- Simulation usage (`sim/pid_simulation.py`)  
- Example step response plot  
- Current status and future enhancements

👉 [Back to the main README on GitHub](https://github.com/OnesmoOgore/embedded-motor-pid-controller#readme)
