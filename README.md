# Conway's Game of Life (EasyX C++ Implementation)

> A robust, modular implementation of cellular automata simulation based on C++ and the EasyX graphics library.

## 📖 Introduction

This project is a high-performance simulation of John Conway's Game of Life. It provides a rich interactive environment to explore cellular automata with various rule sets, boundary conditions, and preset patterns. Built with modular architecture, it separates core logic, rendering, and input handling for maintainability and scalability.

**Project Date:** December 2025
**Platform:** Windows (Visual Studio)

## ✨ Core Features

According to the project specification, this implementation includes:

### 1. Advanced Rule Sets
Beyond the standard Conway rules, the system supports multiple cellular automata variants:
- **Conway's Life**: B3/S23 (Standard)
- **HighLife**: B36/S23 (Known for its replicator)
- **Day & Night**: B3678/S34678 (Symmetrical behavior)
- **Seeds**: B2/S (Explosive growth)

### 2. Boundary Conditions
Customize how the simulation handles the edges of the grid:
- **Toroidal**: Wrap-around (infinite surface simulation).
- **Dead**: Cells die at the edges.
- **Mirror**: Edges act as reflective walls.

### 3. Interactive Controls & View
- **Dynamic Speed**: Adjustable simulation interval (10ms - 500ms).
- **Zooming**: Scale the view from 0.5x to 3.0x.
- **Save/Load**: Native `.sav` file support for saving grid states.
- **RLE Support**: Basic Run Length Encoded pattern parsing.

### 4. Preset Library
Instant instantiation of famous patterns via keyboard shortcuts:
- Glider
- Light Weight Spaceship (LWSS)
- Pulsar
- Gosper Glider Gun

---

## 🎮 Control Reference

| Key / Input | Action | Description |
| :--- | :--- | :--- |
| **Mouse Left** | Toggle Cell | Click to revive or kill a specific cell |
| **Space** / **Z** | Pause / Resume | Toggle simulation state |
| **N** | Next Step | Advance one generation manually (when paused) |
| **R** | Randomize | Fill the grid with random cells |
| **C** | Clear | Kill all living cells |
| **+/-** | Zoom | Adjust view scale (0.5x - 3.0x) |
| **S** | Save | Save current state to file |
| **L** | Load | Load state from file |
| **1 - 6** | Spawn Pattern | 1:Glider, 2:LWSS, 3:Pulsar, 4:Gun, etc. |
| **ESC** | Exit | Terminate the application |

---

## 📂 Project Structure

The project follows a modular design pattern to ensure logic separation:

```text
GameOfLife/
├── main.cpp        // Entry point, main loop, and configuration
├── global.h/.cpp   // Global state variables and shared constants
├── core.h/.cpp     // Core logic (evolution algorithms, neighbor counting)
├── render.h/.cpp   // Graphics rendering (grid, cells, UI overlay)
├── input.h/.cpp    // Input processing (keyboard, mouse handler)
└── saves/          // Directory for save files (.sav)
