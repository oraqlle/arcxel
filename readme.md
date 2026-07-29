# Arcxel

Test bench for analysing different threading architectures for game engine workloads.

## Dependencies

This game relies on raylib which lists it's dependencies on its
[wiki](https://github.com/raysan5/raylib/wiki/raylib-dependencies)

## Building

```sh
cmake -B build
cmake --build build
```

## Roadmap

1. Window Handling
2. Rendering a Triangle and a Quad
3. Basic Instrumentation
    a.  Logging system
    b.  In program timing and measurements
4. 3D Model Loading
5. Rendering Pipeline
6. Physics Simulation
7. Multiple game objects via Entity Component System (ECS)
8. Test Bench Simulation Rules and Configuration
9. Parallelisation of game loop
    a.  Static Partitioning Threading Architecture
    b.  Fine-Grained Task Scheduling Threading Architecture

