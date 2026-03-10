# Roguelike Game

2D tile-based roguelike built with C++17 and raylib.

## Learning Project

This is a learning project for game development. When the user asks how to do something, **guide them through the approach first** rather than jumping straight to implementation. Explain the concepts, trade-offs, and reasoning so they can make informed decisions and learn. Only write code when explicitly asked to implement.

## Build & Run

```bash
./run        # Debug build and run
./run -r     # Release (optimized) build and run
```

This is the primary way to build and run during development. The script handles cmake configure, build, and execution in one step.

Raylib 5.5 is fetched automatically via CMake FetchContent — no manual dependency installation needed.

## Project Structure

```
src/
└── main.cpp    # Entry point
```

## Conventions

- C++17, compiled with CMake 3.20+
- Header includes use path relative to `src/` (e.g. `#include "renderer/renderer.h"`)
- Private member variables use trailing underscore (`tileWidth_`)
- Grid coordinates are integer tile positions; pixel conversion happens inside the renderer
- New `.cpp` files must be added to the `add_executable` list in `CMakeLists.txt`
