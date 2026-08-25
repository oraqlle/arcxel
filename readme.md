# Arcxel

Test bench for analysing different threading architectures for game engine workloads.

## Dependencies

Arcxel relies on raylib, which lists its dependencies on its
[wiki](https://github.com/raysan5/raylib/wiki/raylib-dependencies). raylib is
fetched and built from source, and linked statically so that measured calls into
it do not pay for cross-image dispatch.

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

## Build options

| Option | Default | Effect |
| --- | --- | --- |
| `ARCXEL_PROFILING` | `ON` | Records timing spans. `OFF` compiles every span away. |
| `ARCXEL_LOGGING` | `ON` | `OFF` discards every level at compile time and leaves raylib's own output alone. |
| `ARCXEL_DEBUG_SANITIZERS` | `OFF` | Builds with AddressSanitizer, on the library as well as the executable. |

```sh
cmake -B build -DARCXEL_PROFILING=OFF
```

Measuring the profiler's own cost means building both ways and comparing, so
`ARCXEL_PROFILING` exists to make the second build possible.

## Running

Each run writes its log and its timing samples side by side under `results/`:

```
results/2026-08-08/arcxel-timing-122933.log
results/2026-08-08/arcxel-timing-122933.csv
```

Set `ARCXEL_LOG_LEVEL` to change how much is reported — `trace`, `debug`,
`info` (the default), `warn`, `error`, `fatal` or `off`:

```sh
ARCXEL_LOG_LEVEL=warn ./build/arcxel
```

Release builds discard `trace` and `debug` when compiling, so those two cannot
be re-enabled at runtime.

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

