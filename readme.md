# Arcxel

Test bench for analysing different threading architectures for game engine workloads.

## Dependencies

* [raylib](https://github.com/raysan5/raylib) [v6.0]
* [reactphysics3d](https://github.com/DanielChappuis/reactphysics3d) [7000610a244ea85377e4f4c9bcdf68af0159b595]
* [cxxopts](https://github.com/jarro2783/cxxopts) [v3.3.1]

> Note:
>
> * raylib requires a number of dependencies, please install them according to raylibs
>   [wiki](https://github.com/raysan5/raylib/wiki/raylib-dependencies).
> * reactphysics3d has a build error on MSVC platforms. This is resolved by applying the
>   patch 'patches/reactphysics3d-chrono.patch'.

## Building

```sh
cmake -B build -DARCXEL_THREADING_MODEL=SERIAL
cmake --build build
```

> Note: Threading model options
>
> * SERIAL
> * STATIC_PARTITIONING
> * TASK_BASED

## Running

```sh
arcxel --help
Arcxel Testbed
Usage:
  arcxel [OPTION...]

  -n, --num_objects arg  Number of objects to run simulation with
  -j, --jobs arg         Number of parallel jobs (threads) to run engine with
  -t, --trace arg        Output directory of trace file
  -l, --log arg          Output directory of log file
      --window-name arg  Name of the window
  -h, --help             Show help
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
7. Multiple game objects
8. Test Bench Simulation Rules and Configuration
9. Parallelisation of game loop
    a.  Static Partitioning Threading Architecture
    b.  Fine-Grained Task Scheduling Threading Architecture

