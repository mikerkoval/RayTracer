#include "TriangleMesh.h"
#include "Raytracer.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits>
using namespace std;

thread_local Triangle* TriangleMesh::lastHitTriangle = nullptr;

// ---- string helpers ----
static vector<string> split(const string& s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) elems.push_back(item);
    return elems;
}

// ---- AABB ----
void AABB::expand(const AABB& o) {
    min = Vect(
        fmin(min.getX(), const_cast<Vect&>(o.min).getX()),
        fmin(min.getY(), const_cast<Vect&>(o.min).getY()),
        fmin(min.getZ(), const_cast<Vect&>(o.min).getZ())
    );
    max = Vect(
        fmax(max.getX(), const_cast<Vect&>(o.max).getX()),
        fmax(max.getY(), const_cast<Vect&>(o.max).getY()),
        fmax(max.getZ(), const_cast<Vect&>(o.max).getZ())
    );
}

// Slab method AABB intersection — returns entry t or -1 if miss
double AABB::intersect(const Ray& ray) const {
    Vect o = const_cast<Ray&>(ray).getOrigin();
    Vect d = const_cast<Ray&>(ray).getDirection();

    double tmin = -1e18, tmax = 1e18;

    double ox = o.getX(), oy = o.getY(), oz = o.getZ();
    double dx = d.getX(), dy = d.getY(), dz = d.getZ();
    double minx = const_cast<Vect&>(min).getX(), miny = const_cast<Vect&>(min).getY(), minz = const_cast<Vect&>(min).getZ();
    double maxx = const_cast<Vect&>(max).getX(), maxy = const_cast<Vect&>(max).getY(), maxz = const_cast<Vect&>(max).getZ();

    // X slab
    if (fabs(dx) > 1e-12) {
        double t1 = (minx - ox) / dx;
        double t2 = (maxx - ox) / dx;
        if (t1 > t2) swap(t1, t2);
        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
    } else if (ox < minx || ox > maxx) return -1;

    // Y slab
    if (fabs(dy) > 1e-12) {
        double t1 = (miny - oy) / dy;
        double t2 = (maxy - oy) / dy;
        if (t1 > t2) swap(t1, t2);
        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
    } else if (oy < miny || oy > maxy) return -1;

    // Z slab
    if (fabs(dz) > 1e-12) {
        double t1 = (minz - oz) / dz;
        double t2 = (maxz - oz) / dz;
        if (t1 > t2) swap(t1, t2);
        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
    } else if (oz < minz || oz > maxz) return -1;

    if (tmax < tmin || tmax < 0) return -1;
    return tmin > 0 ? tmin : tmax;
}

// ---- BVH build helpers ----
AABB TriangleMesh::triangleAABB(Triangle* t) const {
    Vect a = t->getA(), b = t->getB(), c = t->getC();
    double ax=a.getX(),ay=a.getY(),az=a.getZ();
    double bx=b.getX(),by=b.getY(),bz=b.getZ();
    double cx=c.getX(),cy=c.getY(),cz=c.getZ();
    return AABB(
        Vect(fmin(ax,fmin(bx,cx)), fmin(ay,fmin(by,cy)), fmin(az,fmin(bz,cz))),
        Vect(fmax(ax,fmax(bx,cx)), fmax(ay,fmax(by,cy)), fmax(az,fmax(bz,cz)))
    );
}

static Vect centroid(Triangle* t) {
    Vect a = t->getA(), b = t->getB(), c = t->getC();
    return Vect((a.getX()+b.getX()+c.getX())/3.0,
                (a.getY()+b.getY()+c.getY())/3.0,
                (a.getZ()+b.getZ()+c.getZ())/3.0);
}

unique_ptr<BVHNode> TriangleMesh::buildBVH(vector<Triangle*>& tris, int start, int end) {
    unique_ptr<BVHNode> node = make_unique<BVHNode>();

    // Compute bounds for this range
    for (int i = start; i < end; i++)
        node->bounds.expand(triangleAABB(tris[i]));

    int count = end - start;
    if (count <= 4) {
        // Leaf
        node->tris.assign(tris.begin() + start, tris.begin() + end);
        return node;
    }

    // Choose split axis: longest extent
    Vect ext = Vect(
        node->bounds.max.getX() - node->bounds.min.getX(),
        node->bounds.max.getY() - node->bounds.min.getY(),
        node->bounds.max.getZ() - node->bounds.min.getZ()
    );
    int axis = 0;
    if (ext.getY() > ext.getX()) axis = 1;
    if (axis == 0 && ext.getZ() > ext.getX()) axis = 2;
    if (axis == 1 && ext.getZ() > ext.getY()) axis = 2;

    // Sort by centroid on chosen axis
    sort(tris.begin() + start, tris.begin() + end, [axis](Triangle* a, Triangle* b) {
        Vect ca = centroid(a), cb = centroid(b);
        double va = axis == 0 ? ca.getX() : (axis == 1 ? ca.getY() : ca.getZ());
        double vb = axis == 0 ? cb.getX() : (axis == 1 ? cb.getY() : cb.getZ());
        return va < vb;
    });

    int mid = start + count / 2;
    node->left  = buildBVH(tris, start, mid);
    node->right = buildBVH(tris, mid,   end);
    return node;
}

// ---- BVH traversal ----
double TriangleMesh::traverseBVH(const BVHNode* node, const Ray& localRay, Triangle*& hitTri) const {
    if (!node) return -1;
    if (node->bounds.intersect(localRay) < 0) return -1;

    if (node->isLeaf()) {
        double best = -1;
        for (Triangle* t : node->tris) {
            double d = t->findIntersection(localRay);
            if (d > 1e-6 && (best < 0 || d < best)) {
                best = d;
                hitTri = t;
            }
        }
        return best;
    }

    Triangle* leftHit  = nullptr;
    Triangle* rightHit = nullptr;
    double lt = traverseBVH(node->left.get(),  localRay, leftHit);
    double rt = traverseBVH(node->right.get(), localRay, rightHit);

    if (lt > 0 && (rt < 0 || lt <= rt)) { hitTri = leftHit;  return lt; }
    if (rt > 0)                          { hitTri = rightHit; return rt; }
    return -1;
}

// ---- rebuild BVH ----
void TriangleMesh::rebuildBVH() {
    bvhRoot = buildBVH(triangles, 0, (int)triangles.size());
}

// ---- MTL loader ----
void TriangleMesh::loadMtl(const string& path, const string& baseDir) {
    ifstream f(path);
    if (!f.is_open()) { cout << "Warning: cannot open MTL: " << path << "\n"; return; }

    string line;
    int cur = -1;
    while (getline(f, line)) {
        // strip carriage returns
        if (!line.empty() && line.back() == '\r') line.pop_back();
        vector<string> arr = split(line, ' ');
        if (arr.empty() || arr[0].empty() || arr[0][0] == '#') continue;

        if (arr[0] == "newmtl" && arr.size() >= 2) {
            materials.push_back(MtlMaterial());
            cur = (int)materials.size() - 1;
            // store name temporarily in a lookup handled below
        } else if (cur < 0) {
            continue;
        } else if (arr[0] == "Kd" && arr.size() >= 4) {
            materials[cur].kd = Color(stod(arr[1]), stod(arr[2]), stod(arr[3]), 0);
        } else if (arr[0] == "Ks" && arr.size() >= 4) {
            // use luminance of Ks as specularity
            materials[cur].ks = (stod(arr[1]) + stod(arr[2]) + stod(arr[3])) / 3.0;
        } else if (arr[0] == "Ns" && arr.size() >= 2) {
            materials[cur].ns = stod(arr[1]);
        } else if (arr[0] == "Ni" && arr.size() >= 2) {
            materials[cur].ni = stod(arr[1]);
        } else if (arr[0] == "d" && arr.size() >= 2) {
            materials[cur].d = stod(arr[1]);
        } else if (arr[0] == "map_Kd" && arr.size() >= 2) {
            string texpath = baseDir + "/" + arr[1];
            try {
                materials[cur].map_kd = make_unique<ImageTexture>(texpath);
            } catch (...) {
                cout << "Warning: could not load texture: " << texpath << "\n";
            }
        }
    }
}

// ---- OBJ loader ----
void TriangleMesh::createMesh(string path) {
    ifstream f(path);
    if (!f.is_open()) { cout << "Unable to open file: " << path << "\n"; return; }

    // Derive base directory for resolving relative MTL/texture paths
    string baseDir = ".";
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != string::npos) baseDir = path.substr(0, lastSlash);

    // MTL name -> index map
    unordered_map<string, int> mtlIndex;
    int currentMtl = -1;  // -1 = no material assigned yet

    string line;
    while (getline(f, line)) {
        // strip carriage returns
        if (!line.empty() && line.back() == '\r') line.pop_back();
        vector<string> arr = split(line, ' ');
        if (arr.empty()) continue;
        if (arr[0] == "mtllib" && arr.size() >= 2) {
            string mtlpath = baseDir + "/" + arr[1];
            int baseMtlIdx = (int)materials.size();
            loadMtl(mtlpath, baseDir);
            // Rebuild name index after loading (we use insertion order)
            // Names are stored sequentially; re-parse the MTL for names only
            ifstream mf(mtlpath);
            string ml;
            int mi = baseMtlIdx;
            while (getline(mf, ml)) {
                if (!ml.empty() && ml.back() == '\r') ml.pop_back();
                vector<string> ma = split(ml, ' ');
                if (ma.size() >= 2 && ma[0] == "newmtl") {
                    mtlIndex[ma[1]] = mi++;
                }
            }
        } else if (arr[0] == "usemtl" && arr.size() >= 2) {
            auto it = mtlIndex.find(arr[1]);
            currentMtl = (it != mtlIndex.end()) ? it->second : -1;
        } else if (arr[0] == "v" && arr.size() >= 4) {
            corners.push_back(Vect(stod(arr[1]), stod(arr[2]), stod(arr[3])));
        } else if (arr[0] == "vt" && arr.size() >= 3) {
            textures.push_back(Vect(stod(arr[1]), stod(arr[2]), 0));
        } else if (arr[0] == "vn" && arr.size() >= 4) {
            vnormals.push_back(Vect(stod(arr[1]), stod(arr[2]), stod(arr[3])));
        } else if (arr[0] == "f" && arr.size() >= 4) {
            // Parse all face vertices (supports quads and n-gons)
            struct FaceVert { int v, vt, vn; };
            vector<FaceVert> fverts;
            for (int i = 1; i < (int)arr.size(); i++) {
                if (arr[i].empty()) continue;
                vector<string> p = split(arr[i], '/');
                FaceVert fv;
                fv.v  = stoi(p[0]) - 1;
                fv.vt = (p.size() > 1 && !p[1].empty()) ? stoi(p[1]) - 1 : -1;
                fv.vn = (p.size() > 2 && !p[2].empty()) ? stoi(p[2]) - 1 : -1;
                fverts.push_back(fv);
            }

            // Fan triangulation: (0,1,2), (0,2,3), (0,3,4), ...
            for (int i = 1; i + 1 < (int)fverts.size(); i++) {
                FaceVert& fa = fverts[0];
                FaceVert& fb = fverts[i];
                FaceVert& fc = fverts[i + 1];

                bool hasNormals = fa.vn >= 0 && fb.vn >= 0 && fc.vn >= 0;
                bool hasUVs     = fa.vt >= 0 && fb.vt >= 0 && fc.vt >= 0;

                Vect na = hasNormals ? vnormals[fa.vn] : Vect(0,1,0);
                Vect nb = hasNormals ? vnormals[fb.vn] : Vect(0,1,0);
                Vect nc = hasNormals ? vnormals[fc.vn] : Vect(0,1,0);

                // Use old Magick texture path only if explicitly set via constructor
                if (setText && texture && hasUVs) {
                    triangleOs.push_back(Triangle(
                        &corners[fa.v], &corners[fb.v], &corners[fc.v],
                        na, nb, nc,
                        textures[fa.vt], textures[fb.vt], textures[fc.vt], texture));
                } else {
                    triangleOs.push_back(Triangle(
                        &corners[fa.v], &corners[fb.v], &corners[fc.v],
                        na, nb, nc, color));
                }
                // Store UV coords and material index for MTL texturing
                triMaterial.push_back(currentMtl);
                Vect uva = hasUVs ? textures[fa.vt] : Vect(0,0,0);
                Vect uvb = hasUVs ? textures[fb.vt] : Vect(0,0,0);
                Vect uvc = hasUVs ? textures[fc.vt] : Vect(0,0,0);
                triUVs.push_back({uva, uvb, uvc});
            }
        }
    }
    f.close();

    for (Triangle& t : triangleOs) triangles.push_back(&t);

    // Bounding radius in local space
    boundingRadius = 0;
    for (Vect& v : corners) {
        double m = v.magnitude();
        if (m > boundingRadius) boundingRadius = m;
    }

    // Build BVH
    bvhRoot = buildBVH(triangles, 0, (int)triangles.size());
}

// ---- transform helpers ----
static Matrix4x4 buildInv(Vect pos, Matrix4x4 rot) {
    Matrix4x4 t;
    t.translate(pos.getX(), pos.getY(), pos.getZ());
    return t.mult(rot).inverse();
}

// ---- findIntersection ----
double TriangleMesh::findIntersection(Ray ray) {
    Matrix4x4 inv = buildInv(position, rotation);

    Vect p0 = ray.getOrigin();
    Vect p1 = p0.add(ray.getDirection());
    Vect lp0 = inv.mult(p0);
    Vect lp1 = inv.mult(p1);
    Vect ldir = lp1.add(lp0.negative());
    double ldir_len = ldir.magnitude();
    Ray localRay(lp0, ldir.normalize());

    // Bounding sphere early-out
    Sphere bsphere(Vect(0,0,0), boundingRadius, Color());
    if (bsphere.findIntersection(localRay) == -1) return -1;

    Triangle* hitTri = nullptr;
    double best = traverseBVH(bvhRoot.get(), localRay, hitTri);
    lastHitTriangle = hitTri;
    // Convert local-space t back to world-space t
    return best > 0 ? best / ldir_len : best;
}

// ---- getNormalAt ----
Vect TriangleMesh::getNormalAt(Vect worldPoint) {
    Matrix4x4 inv = buildInv(position, rotation);
    Vect localPoint = inv.mult(worldPoint);

    Matrix4x4 rotInv = rotation.inverse();
    Matrix4x4 tinv = rotInv.transpose();

    if (lastHitTriangle) {
        Vect localNorm = lastHitTriangle->getNormalAt(localPoint);
        return tinv.mult(localNorm).normalize();
    }

    // Fallback: linear scan (should rarely happen)
    for (Triangle* tp : triangles) {
        if (pointInTriangle(*tp, localPoint)) {
            Vect localNorm = tp->getNormalAt(localPoint);
            return tinv.mult(localNorm).normalize();
        }
    }
    return Vect(0, 1, 0);
}

// ---- getColor ----
Color TriangleMesh::getColor(Vect worldPoint) {
    Matrix4x4 inv = buildInv(position, rotation);
    Vect localPoint = inv.mult(worldPoint);

    Triangle* tri = lastHitTriangle;
    if (!tri) {
        for (Triangle* tp : triangles) {
            if (pointInTriangle(*tp, localPoint)) { tri = tp; break; }
        }
    }
    if (!tri) return color;

    // Find triangle index
    int idx = (int)(tri - &triangleOs[0]);

    // Try MTL material first
    if (idx >= 0 && idx < (int)triMaterial.size()) {
        int mi = triMaterial[idx];
        if (mi >= 0 && mi < (int)materials.size()) {
            MtlMaterial& mat = materials[mi];
            if (mat.map_kd) {
                // Interpolate UV using barycentric coords
                Vect n = tri->getTriangleNormal();
                Vect A = tri->getA(), B = tri->getB(), C = tri->getC();
                Vect ba = B.add(A.negative()).negative();
                Vect ca = C.add(A.negative()).negative();
                Vect bp = B.add(localPoint.negative()).negative();
                Vect cp = C.add(localPoint.negative()).negative();
                Vect ap = A.add(localPoint.negative()).negative();
                double areaABC = fabs(n.dotProduct(ba.crossProduct(ca)));
                double u = fabs(n.dotProduct(bp.crossProduct(cp))) / areaABC;
                double v = fabs(n.dotProduct(cp.crossProduct(ap))) / areaABC;
                double w = 1.0 - u - v;
                auto& uvs = triUVs[idx];
                double tu = uvs[0].getX()*u + uvs[1].getX()*v + uvs[2].getX()*w;
                double tv = uvs[0].getY()*u + uvs[1].getY()*v + uvs[2].getY()*w;
                return mat.map_kd->getColor(tu, 1.0 - tv);
            }
            return mat.kd;
        }
    }

    return tri->getColor(localPoint);
}

// ---- point-in-triangle (local space) — kept as fallback ----
bool TriangleMesh::pointInTriangle(Triangle& t, Vect p) {
    Vect normal = t.getTriangleNormal();
    Vect A = t.getA(), B = t.getC(), C = t.getB();

    Vect QA(p.getX()-A.getX(), p.getY()-A.getY(), p.getZ()-A.getZ());
    double onPlane = QA.dotProduct(normal);

    Vect ba = B.add(A.negative()).negative();
    Vect ca = C.add(A.negative()).negative();
    Vect ap = A.add(p.negative()).negative();
    Vect bp = B.add(p.negative()).negative();
    Vect cp = C.add(p.negative()).negative();

    double areaABC = fabs(normal.dotProduct(ba.crossProduct(ca)));
    double areaPBC = fabs(normal.dotProduct(bp.crossProduct(cp)));
    double areaPCA = fabs(normal.dotProduct(cp.crossProduct(ap)));

    double u = areaPBC / areaABC;
    double v = areaPCA / areaABC;
    double w = 1.0 - u - v;

    double eps = 1e-4;
    return (u >= -eps && v >= -eps && w >= -eps &&
            onPlane > -eps && onPlane < eps);
}

bool TriangleMesh::getCL() { return clearLight; }

// ---- constructors ----
TriangleMesh::TriangleMesh() {
    center = Vect(0,0,0); color = Color(1,1,1,0);
    setText = false; clearLight = false; boundingRadius = 0;
    rotation = Matrix4x4(); position = Vect();
    createMesh("sphere.obj");
}
TriangleMesh::TriangleMesh(string file, Color c) {
    center = Vect(0,0,0); color = c;
    setText = false; clearLight = false; boundingRadius = 0;
    rotation = Matrix4x4(); position = Vect();
    createMesh(file);
}
TriangleMesh::TriangleMesh(string file, Color c, bool l) {
    center = Vect(0,0,0); color = c;
    setText = false; clearLight = l; boundingRadius = 0;
    rotation = Matrix4x4(); position = Vect();
    createMesh(file);
}
TriangleMesh::TriangleMesh(string file, Magick::Image* c) {
    center = Vect(0,0,0); texture = c;
    setText = true; clearLight = false; boundingRadius = 0;
    rotation = Matrix4x4(); position = Vect();
    createMesh(file);
}
TriangleMesh::TriangleMesh(string file, Magick::Image* c, bool l) {
    center = Vect(0,0,0); texture = c;
    setText = true; clearLight = l; boundingRadius = 0;
    rotation = Matrix4x4(); position = Vect();
    createMesh(file);
}
