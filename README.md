
# Hades mod subsystem

The projects implements a mod subsystem for Hades 1.

# Building

## Using CMake for Windows x64

```powershell
cmake -A x64 . -B build_msvc
cmake --build build_msvc --target install --config Release
```

Binary files are located in the `bin` folder.

# Installation

Copy files from the `bin` folder to the `Hades/x64Vk` or `Hades/x64` folder, depending on the used game version
