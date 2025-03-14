
# Hades mod subsystem

The projects implements a mod subsystem for Hades 1.

# Building

## Using CMake for Windows x64

```powershell
cmake -A x64 . -B build_msvc
cmake --build build_msvc --config Release
```

Binary files are located in the `bin` folder.

# Installation

Copy files the `bin` folder to the `Hades/x64Vk` folder.
