# ANARI-OSPRay

Translation layer from Khronos® [ANARI™](https://www.khronos.org/anari)
to Intel® [OSPRay](https://www.ospray.org): ANARILibrary and ANARIDevice
"`ospray`".

## Status

This is an experimental project, v0.1, implementation of the ANARI API
is incomplete.

Features and extensions that should work:
- Geometries (with explicit `primitive.index`): `triangle`, `quad`,
  `sphere`, `curve` (constant `radius` only)
- Materials: `matte`, `transparentMatte`, and as extension all OSPRay
  materials (`obj`, `principled`, `carPaint`, `glass`, `thinGlass`,
  `luminous`, ...)
- Spatial Fields: `structuredRegular`, and as extension all OSPRay
  volumes (`structuredSpherical`, `amr`, `unstructured`, `particle`)
- Lights
- Cameras
- Instances
- `KHR_AREA_LIGHT`
- `KHR_AUXILIARY_BUFFERS` of type `FLOAT32_VEC`
- `KHR_FRAME_COMPLETION_CALLBACK`
- `KHR_STOCHASTIC_RENDERING)`
- `KHR_TRANSFORMATION_MOTION_BLUR`

Not implemented yet:
- Geometries: `cone`, `cylinder`, `primitive.radius` of `curve`,
  implicit `primitive.index`, i.e., primitive soups
- Object introspection
- Samplers
- Attribute selection and routing for Materials
- `{color|opacity}.position` array of `scivis` Volume
- stridede Arrays
- (un)mapping Arrays
- `KHR_DEVICE_SYNCHRONIZATION`
- ...

## Requirements

- CMake
- C++11 compiler
- [ANARI-SDK](https://github.com/KhronosGroup/ANARI-SDK)
- [OSPRay](https://www.github.com/ospray/ospray) v2.9.0, i.e., [devel
  branch](https://github.com/ospray/ospray/tree/devel)

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
