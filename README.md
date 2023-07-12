# ANARI-OSPRay

Translation layer from Khronos® [ANARI™](https://www.khronos.org/anari)
to Intel® [OSPRay](https://www.ospray.org): ANARILibrary and ANARIDevice
"`ospray`".

## Requirements

- CMake
- C++11 compiler
- [ANARI-SDK](https://github.com/KhronosGroup/ANARI-SDK) v0.4.0 or later
- [OSPRay](https://www.github.com/ospray/ospray) v2.12.0+
- Python 3

## Building

Build with:

```sh
git clone https://github.com/ospray/anari-ospray.git
cd anari-ospray
mkdir build
cd build
cmake ..
cmake --build .
```

Or using the CMake superbuild in `superbuild`, which fetches the
dependencies.

## Using

Run the ANARI examples with selected `ospray` library, e.g.
```bash
export ANARI_LIBRARY=ospray
anariViewer
```
