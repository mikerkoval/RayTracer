# AnimationProject

A CPU raytracer written in C++ that renders photorealistic scenes and exports frame sequences as PNG images or animated GIFs.

![Teapot rotation](output.gif)

## Features

- **Geometric primitives:** sphere, plane, cylinder, cone, pyramid, rectangle, triangle
- **Triangle mesh import:** OBJ files with vertex normals and texture coordinates (smooth shading via barycentric interpolation)
- **Smooth normal generation:** computes area-weighted vertex normals from geometry, eliminating patch seams
- **BVH acceleration:** bounding volume hierarchy with AABB slab intersection, reducing triangle tests from O(n) to O(log n)
- **Reflections:** recursive mirror reflections with configurable specularity per material
- **Lighting:** point lights, ambient, diffuse, specular/glossy reflections, shadows
- **Procedural textures:** Perlin noise and Worley noise via FastNoise
- **Transforms:** full 4×4 matrix pipeline (rotation, translation, scale) with automatic inverse computation
- **Animation:** per-frame rotation interpolation exported as PNG sequences; GIF assembly via ImageMagick
- **Multithreaded rendering:** column-parallel rendering across all available hardware threads
- **Regression tests:** CTest suite with pixel-exact image comparison against reference renders

## Build & Run

Requires CMake ≥ 3.5 and ImageMagick with Magick++ headers.

```bash
bash run.sh
```

This will build, run the test suite, render all frames, and assemble `output.gif`.

Or step by step:

```bash
cmake -B build .
cmake --build build
ctest --test-dir build
./build/Animation
convert -delay 3 -loop 0 output/frame*.png output.gif
```

## Testing

```bash
ctest --test-dir build
```

Tests render known scenes and compare pixel-for-pixel against reference images in `tests/references/`. To regenerate a reference after an intentional change, delete the relevant file from `tests/references/` and rerun.

## Project structure

```
raytracer/
  include/   — headers for all shapes, lights, and the rendering engine
  src/       — implementations
obj/         — OBJ mesh files
tests/
  render_test.cpp   — test renderer binary
  compare.sh        — renders and compares against reference
  references/       — reference PNG images for regression testing
output/      — rendered frames
```
