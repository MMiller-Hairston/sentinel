# Sentinel

Sentinel is going to be a crossplatform C++ game built on SDL 3.

## Prerequisites

- CMake 3.25 or newer
- Ninja
- A C++23 compiler
- [vcpkg](https://github.com/microsoft/vcpkg)

Set `VCPKG_ROOT` to the directory where vcpkg is installed. The checked-in
`vcpkg.json` manifest installs dependencies automatically when CMake configures the
project.

```sh
export VCPKG_ROOT=/path/to/vcpkg
```

## Build

Configure and build the Debug configuration:

```sh
cmake --preset debug
cmake --build --preset debug
```
