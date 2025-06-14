# Diploma Project

## Table of Contents
- [Introduction](#introduction)  
- [Features](#features)  
- [Usage](#usage)  
- [Modules](#modules)  
- [Customization](#customization)  
- [Controls](#controls)    
- [Media](#media)

---

## Introduction

This project is a simulation suite developed in C++ for academic and experimental purposes. It focuses on three core modules: **vision and light detection**, **water behavior with physical interaction**, and **particle-based fire simulation**. These simulations are implemented using **SFML** and demonstrate real-time rendering, interaction, and configurable behavior.

---

## Features

- **Raycasting Vision Simulation**: Models field-of-view and visibility using raycasting from a central point.
- **Water Simulation with Balls**: Represents liquid behavior through sphere-based particle motion and collision.
- **Particle Fire System**: Creates realistic flame behavior using dynamic particle properties and lifetimes.
- **Interactive Environment**: User-controlled interactions such as spawning, movement, and toggling effects.
- **Modular Codebase**: Easy to add or switch simulation modes via key commands.
- **SFML Integration**: Leverages SFML for rendering, event handling, and real-time performance.
- **Performance Logging**: Track particle count and simulation updates in real time.

---

## Usage

1. Clone the repository:
   ```bash
   git clone https://github.com/tanyavasilkiv21/Diploma.git
   ```

2. Build using any C++17-compatible compiler and link with SFML. Example using g++:
   ```bash
   g++ -std=c++17 main.cpp -o SimulationApp -lsfml-graphics -lsfml-window -lsfml-system
   ```

3. Run the application:
   ```bash
   ./SimulationApp
   ```

---

## Modules

### 1. Vision (Raycasting)
- Simulates how an observer sees in a 2D environment.
- Casts rays in all directions to detect intersections with boundaries.

### 2. Water (Balls Simulation)
- Simulates fluid-like motion using gravity and object collisions.
- Water is represented as multiple bouncing and interacting spheres.

### 3. Fire (Particle System)
- Uses a particle emitter with configurable behaviors (e.g., Spiral, Explosion, Fountain).
- Dynamic and visually realistic fire animation.

---

## Customization

- **Particle Types**: Easily switch between visual effects by modifying the `ParticleStyle` enum.
- **Add New Modes**: Add a new simulation by defining a new mode class and switching logic in `main.cpp`.
- **Simulation Settings**: Adjust frame rate, particle limits, spawn rate, and other constants directly in source code.

---

## Controls

General controls for all modules:

- `1` – Fire particle system  
- `2` – Water simulation with balls  
- `3` – Raycasting vision  
- `Esc` – Quit application  

Fire particle system:

- `Left Mouse Click` – Spawn particle system at cursor location.

Water simulation with balls:

- `Right Mouse Click` – Spawn a ball at cursor location.

Raycasting vision:

- `T` – Toggle polygon drawing on/off.
- `R` – Toggle ray drawing on/off.


---

## Media

#### Raycasting vision:

![](media/gif1.gif)

#### Water simulation with balls:
![](media/gif2.gif)

![](media/gif3.gif)

#### Fire particle system:

![](media/gif4.gif)

![](media/gif5.gif)

![](media/gif6.gif)


