# GlassOverFlow

<p align="center">
  <img src="https://github.com/user-attachments/assets/fae46302-0c08-46e3-9a99-74f48ca1ce4d" alt="Game Picture">
</p>

## Overview

GlassOverFlow is a real-time 2D fluid simulation game developed by a team of three enthusiasts. The game is built from scratch using the OpenGL API.

## Description

The source code is divided into three parts: the engine, the physics solver, and the game itself.

### The Engine

The Engine structure is inspired by an early version of the [Hazel Game Engine](https://github.com/TheCherno/Hazel). It features OpenGL class abstraction, a 2D renderer, and some primitives like squares, circles, and rendering properties. It also features a handling of user events, a camera system, and position transforms.

### The Physics Solver

The solver implements the [DFSPH](https://dl.acm.org/doi/10.1145/2786784.2786796) method and features various kernels:

- **Density**:

$$
\begin{align}
  W_{\text{poly6}}(r, h) &= \frac{315}{64\pi h^9} (h^2 - |r|^2)^3 \\
  \nabla W_{\text{poly6}}(r, h) &= -\frac{945}{32\pi h^9}r(h^2 - |r|^2)^2 \\
  \nabla^2 W_{\text{poly6}}(r, h) &= -\frac{945}{32\pi h^9}(h^2 - |r|^2)(3h^2 - 7|r|^2)
\end{align}
$$

- **Pressure**: (be careful in 0)

$$
\begin{align}
  W_{\text{spiky}}(r, h) &= \frac{15}{\pi h^6}(h-|r|)^3 \\
  \nabla W_{\text{spiky}}(r, h) &= -\frac{45}{\pi h^6}\frac{r}{|r|}(h-|r|)^2 \\
  \nabla^2 W_{\text{spiky}}(r, h) &= -\frac{90}{\pi h^6}\frac{1}{|r|}(h-|r|)(h-2|r|)
\end{align}
$$

- **Viscosity**:

$$
\begin{align}
W_\text{vis}(r, h) &= \frac{15}{2\pi h^3}\left(-\frac{|r|^3}{2h^3}+\frac{|r|^2}{h^2}+\frac{h}{2|r|}-1\right) \\
\nabla W_\text{vis}(r, h) &= \frac{15}{2\pi h^3}r\left(-\frac{3|r|}{2h^3}+\frac{2}{h^2}-\frac{h}{2|r|^3}\right) \\
\nabla^2 W_\text{vis}(r, h) &= \frac{45}{\pi h^6}(h-|r|)
\end{align}
$$

It is capable to handle rigid structures that will interact with the fluid:

<p align="center">
  <img src="https://github.com/user-attachments/assets/c05dfedc-558d-4da2-aab6-614b6865623f" alt="Rigid body" width="50%">
</p>

It is also capable of handling different fluids with different parameters:

<p align="center">
  <img src="https://github.com/user-attachments/assets/3986c6d7-88bf-404c-91a7-1191b319f7dd" alt="Multiple fluids" width="100%">
</p>

### The Game

The game features several levels to showcase the different features. The rendering of the different levels is done using several compute passes to get the 'pixel art' style. It uses some spatial hashing and computes the color of the pixels by using the SPH method idea.

<p align="center">
  <img src="https://github.com/user-attachments/assets/d4f260cb-ad21-476b-abf5-ce24be6c8cfc" alt="Direct particle drawing" width="49%">
  <img src="https://github.com/user-attachments/assets/de114a0e-9a39-4ccf-921d-596755e4c269" alt="Simple pixel art render pass" width="49%">
  <img src="https://github.com/user-attachments/assets/e52e5e03-1792-437a-a285-1b64f9e4b22d" alt="Density computation for rendering" width="49%">
  <img src="https://github.com/user-attachments/assets/a3df8367-a7e5-4a1e-9f07-812e7d972ebd" alt="Final rendering" width="49%">
</p>

From left to right and top to bottom: Rough particle rendering; Simple pixel art render pass; Per pixel density computation; Final rendering

## Getting Started

To build the project, follow these steps:

```bash
mkdir build
cmake -B build
make -C build
```

To update submodules, run:

```bash
git submodule init
git submodule update
```

## Acknowledgments

We would like to thank Prof. Kiwon Um for being our supervisor for this project.
