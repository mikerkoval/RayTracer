#!/usr/bin/env python3
"""
Compute smooth vertex normals for an OBJ file and write them back.
Faces are triangulated (quads -> tris). Normals are area-weighted averages
of all face normals sharing each vertex.
"""

import sys
import math
from collections import defaultdict

def parse_obj(path):
    verts = []
    texcoords = []
    faces = []  # list of [(vi, ti), ...] per face (0-indexed)
    header_lines = []
    in_header = True

    with open(path) as f:
        for line in f:
            line = line.rstrip('\n')
            parts = line.split()
            if not parts:
                if in_header:
                    header_lines.append(line)
                continue
            if parts[0] == 'v':
                in_header = False
                verts.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif parts[0] == 'vt':
                texcoords.append((float(parts[1]), float(parts[2])))
            elif parts[0] == 'f':
                in_header = False
                raw = parts[1:]
                indices = []
                for token in raw:
                    sp = token.split('/')
                    vi = int(sp[0]) - 1
                    ti = int(sp[1]) - 1 if len(sp) > 1 and sp[1] else -1
                    indices.append((vi, ti))
                # triangulate fan
                for i in range(1, len(indices) - 1):
                    faces.append([indices[0], indices[i], indices[i+1]])
            elif parts[0] in ('mtllib', 'o', 'g', 'usemtl', 's', '#'):
                if in_header:
                    header_lines.append(line)

    return verts, texcoords, faces, header_lines

def cross(a, b):
    return (
        a[1]*b[2] - a[2]*b[1],
        a[2]*b[0] - a[0]*b[2],
        a[0]*b[1] - a[1]*b[0],
    )

def sub(a, b):
    return (a[0]-b[0], a[1]-b[1], a[2]-b[2])

def length(v):
    return math.sqrt(v[0]**2 + v[1]**2 + v[2]**2)

def normalize(v):
    l = length(v)
    if l == 0:
        return (0.0, 0.0, 1.0)
    return (v[0]/l, v[1]/l, v[2]/l)

def compute_smooth_normals(verts, faces):
    accum = defaultdict(lambda: [0.0, 0.0, 0.0])

    for tri in faces:
        vi0, vi1, vi2 = tri[0][0], tri[1][0], tri[2][0]
        a = verts[vi0]
        b = verts[vi1]
        c = verts[vi2]
        ab = sub(b, a)
        ac = sub(c, a)
        fn = cross(ab, ac)  # area-weighted (not normalized)
        for vi in (vi0, vi1, vi2):
            accum[vi][0] += fn[0]
            accum[vi][1] += fn[1]
            accum[vi][2] += fn[2]

    normals = {}
    for vi, n in accum.items():
        normals[vi] = normalize(tuple(n))
    return normals

def write_obj(path, verts, texcoords, faces, normals, header_lines):
    # Build a compact list of normals in vertex order
    norm_list = []
    vi_to_ni = {}
    for vi in range(len(verts)):
        if vi in normals:
            vi_to_ni[vi] = len(norm_list)
            norm_list.append(normals[vi])
        else:
            vi_to_ni[vi] = len(norm_list)
            norm_list.append((0.0, 1.0, 0.0))

    with open(path, 'w') as f:
        for line in header_lines:
            f.write(line + '\n')
        f.write('\n')

        for v in verts:
            f.write(f'v {v[0]:.6f} {v[1]:.6f} {v[2]:.6f}\n')
        f.write('\n')

        for tc in texcoords:
            f.write(f'vt {tc[0]:.6f} {tc[1]:.6f}\n')
        f.write('\n')

        for n in norm_list:
            f.write(f'vn {n[0]:.6f} {n[1]:.6f} {n[2]:.6f}\n')
        f.write('\n')

        for tri in faces:
            tokens = []
            for vi, ti in tri:
                ni = vi_to_ni[vi]
                if ti >= 0:
                    tokens.append(f'{vi+1}/{ti+1}/{ni+1}')
                else:
                    tokens.append(f'{vi+1}//{ni+1}')
            f.write('f ' + ' '.join(tokens) + '\n')

def normalize_verts(verts, target_height=3.15):
    """Rescale so Y spans [0, target_height], centered on X/Z."""
    xs = [v[0] for v in verts]
    ys = [v[1] for v in verts]
    zs = [v[2] for v in verts]
    scale = target_height / (max(ys) - min(ys))
    cx = (max(xs) + min(xs)) / 2
    cz = (max(zs) + min(zs)) / 2
    min_y = min(ys)
    return [((v[0] - cx) * scale, (v[1] - min_y) * scale, (v[2] - cz) * scale) for v in verts]

if __name__ == '__main__':
    src = sys.argv[1] if len(sys.argv) > 1 else 'obj/teapot.obj'
    dst = sys.argv[2] if len(sys.argv) > 2 else src

    verts, texcoords, faces, header_lines = parse_obj(src)
    print(f"Loaded {len(verts)} verts, {len(texcoords)} texcoords, {len(faces)} tris")
    verts = normalize_verts(verts)
    normals = compute_smooth_normals(verts, faces)
    print(f"Computed {len(normals)} vertex normals")
    write_obj(dst, verts, texcoords, faces, normals, header_lines)
    print(f"Written to {dst}")
