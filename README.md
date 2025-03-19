# Simulations
This repository contains 3 separate physical simulation. They are based on numerical solution of equations of motion. The simulations are computed on a side thread while main thread handles graphics and communication. As a consequence the simulation and it's visualization can run at different rates.
The camera orbits center of the scene. To spin around hold ctrl + lmb and drag mouse. To zoom hold ctrl and scroll up/down. Skip button allows to advance the simulation by a set number of frames.

## Jelly
[jelly2.webm](https://github.com/user-attachments/assets/b991bd24-df52-43ea-abad-d82998e0eee6)

Soft body simulation approximated by a lattice of 64 point massess interconnected with springs. Control points in corners are connected by additional springs to control frame which allows moving and rotating soft body. The surface of softbody is rendered using 6 bicubic Bezier patches that are constructed on control points. Simulation also allows loading custom model in OBJ format. It's being deformed based on Bernstein control points. Simulation parameters:
- mass - mass of single control point
- c1 - stiffness of springs interconnecting control points
- c2 - stiffness of springs connecting control frame to control points
- k - damping coefficient of the medium
- distortion - amount of random position shift that is applied to single control point when **Distort** button is pressed
- gravity - vector defining force of gravity
- mu - coefficient that defines elasticity of collisions (0 - inelastic collision, 1 - elastic collision)
- collision model - **full component modification** means all velocity components are modified during collision and **velocity component modification** means only one component is affected by collision
- position - XYZ position of soft body
- orientation - RPY representation of soft body rotation  


## Whirligig
[whirligig.webm](https://github.com/user-attachments/assets/7eaba413-769e-4de9-b79a-e3a73ce06bde)

Simulation of rigid body gyration. Trajectory tracing is especially useful for observation of phenomena like nutation. System's motion is described by Euler's equations. They are solved numerically using 4th order Runge-Kutta method. Simulation parameters:
- angular velocity - velocity of rotation along cube's diagonal
- cube tilt - the angle between Y axis and cube's diagonal measures in XY plane
- cube size - length of cube edge
- cube density - self explanatory
- dt - simulation frame duration and size of integration step
- g(t) - type of gravity force function (const by default)
- path length - number of points used for tracing cube's trajectory

## Spring
[spring.webm](https://github.com/user-attachments/assets/bf8c8312-a740-4f71-92b7-8cc09d35d58a)

Simulation of point mass attached to a spring. All interesting simulation properties are plotted on three graphs. Visualization in the middle is a representation of how the system would behave. Spring geometry is generated on the fly in geometry shader using helix parametric equation. Differential equations of motion derived from spring equation are solved with Euler method. Simulation parameters:
- x0 - starting weight position
- v0 - starting weight velocity
- dt - simulation frame duration and size of integration step
- m - mass
- k - damping coefficient of the medium
- c - stiffness of spring
- w(t) - steering of the top of the spring, for const function it's stationary, for harmonic function the movement looks a bit like a yoyo
- h(t) - damping force

## Building

Building has been tested on Linux and Windows.

To download all dependencies make sure to add `--recursive` flag to `git clone` command. You can also run `git submodule update --init --recursive` in an already cloned repository.

To build project run:
```
cmake -B build -S .
cmake --build build
```

This will produce executables `build/spring` `build/whirligig` and `build/jelly` (or `build\Release\spring.exe` `build\Release\whirligig.exe` and `build\Release\jelly.exe` on Windows).