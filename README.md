# AnimationProject

A CPU raytracer written in C++ that renders photorealistic scenes and exports frame sequences as PNG images or animated GIFs.

![Teapot rotation](rotation_no_blur.gif)

## Features

- **Geometric primitives:** sphere, plane, cylinder, cone, pyramid, rectangle, triangle
- **Triangle mesh import:** OBJ files with vertex normals and texture coordinates (smooth shading via barycentric interpolation)
- **Physically-based optics:** Fresnel reflection/refraction using Snell's law with critical angle detection
- **Lighting:** point lights, ambient, specular/glossy reflections, soft shadows
- **Procedural textures:** Perlin noise and Worley noise via FastNoise
- **Transforms:** full 4×4 matrix pipeline (rotation, translation, scale) with automatic inverse computation
- **Animation:** per-frame rotation interpolation exported as PNG sequences; GIF assembly via ImageMagick

## Samples

| Scene | Preview |
|-------|---------|
| Teapot 360° spin | `rotation_no_blur.gif` |
| Motion blur | `rotation_blur.gif` |
| Cone rotation | `cone_rotation.gif` |
| Cylinder rotation | `cylinder_rotation.gif` |

## Build

Requires CMake ≥ 3.5 and ImageMagick with Magick++ headers.

```bash
cmake -B build .
cmake --build build
./build/Animation
```

Rendered frames are written to `output/`.

## Project structure

```
raytracer/
  include/   — headers for all shapes, lights, and the rendering engine
  src/       — implementations
obj/         — OBJ mesh files
pictures/    — rendered frame output
```
