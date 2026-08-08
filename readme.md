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

Builds default to `Release` so measurements are never taken from an unoptimised
binary. Override explicitly if needed:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

Multi-config generators (Visual Studio, Xcode) pick the configuration when
building rather than when configuring, so the default above does not apply and
the configuration must be named explicitly:

```sh
cmake --build build --config Release
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

