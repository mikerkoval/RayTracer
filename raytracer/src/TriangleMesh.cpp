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
    auto node = make_unique<BVHNode>();

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
            if (d > 0.01 && (best < 0 || d < best)) {
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

// ---- OBJ loader ----
void TriangleMesh::createMesh(string path) {
    ifstream f(path);
    if (!f.is_open()) { cout << "Unable to open file: " << path << "\n"; return; }

    string line;
    while (getline(f, line)) {
        auto arr = split(line, ' ');
        if (arr.empty()) continue;
        if (arr[0] == "v" && arr.size() >= 4) {
            corners.push_back(Vect(stod(arr[1]), stod(arr[2]), stod(arr[3])));
        } else if (arr[0] == "vt" && arr.size() >= 3) {
            textures.push_back(Vect(stod(arr[1]), stod(arr[2]), 0));
        } else if (arr[0] == "vn" && arr.size() >= 4) {
            vnormals.push_back(Vect(stod(arr[1]), stod(arr[2]), stod(arr[3])));
        } else if (arr[0] == "f" && arr.size() >= 4) {
            auto pa = split(arr[1], '/');
            auto pb = split(arr[2], '/');
            auto pc = split(arr[3], '/');
            int c1 = stoi(pa[0]) - 1;
            int c2 = stoi(pb[0]) - 1;
            int c3 = stoi(pc[0]) - 1;
            int n1 = stoi(pa[2]) - 1;
            int n2 = stoi(pb[2]) - 1;
            int n3 = stoi(pc[2]) - 1;

            if (setText && pa.size() > 1 && !pa[1].empty()) {
                int t1 = stoi(pa[1]) - 1;
                int t2 = stoi(pb[1]) - 1;
                int t3 = stoi(pc[1]) - 1;
                triangleOs.push_back(Triangle(
                    &corners[c1], &corners[c2], &corners[c3],
                    vnormals[n1], vnormals[n2], vnormals[n3],
                    textures[t1], textures[t2], textures[t3], texture));
            } else {
                triangleOs.push_back(Triangle(
                    &corners[c1], &corners[c2], &corners[c3],
                    vnormals[n1], vnormals[n2], vnormals[n3], color));
            }
        }
    }
    f.close();

    for (auto& t : triangleOs) triangles.push_back(&t);

    // Bounding radius in local space
    boundingRadius = 0;
    for (auto& v : corners) {
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
    Vect ldir = lp1.add(lp0.negative()).normalize();
    Ray localRay(lp0, ldir);

    // Bounding sphere early-out
    Sphere bsphere(Vect(0,0,0), boundingRadius, Color());
    if (bsphere.findIntersection(localRay) == -1) return -1;

    Triangle* hitTri = nullptr;
    double best = traverseBVH(bvhRoot.get(), localRay, hitTri);
    lastHitTriangle = hitTri;
    return best;
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
    for (auto* tp : triangles) {
        if (pointInTriangle(*tp, localPoint)) {
            Vect localNorm = tp->getNormalAt(localPoint);
            return tinv.mult(localNorm).normalize();
        }
    }
    return Vect(0, 1, 0);
}

// ---- getColor ----
Color TriangleMesh::getColor(Vect worldPoint) {
    if (lastHitTriangle) {
        Matrix4x4 inv = buildInv(position, rotation);
        Vect localPoint = inv.mult(worldPoint);
        return lastHitTriangle->getColor(localPoint);
    }

    Matrix4x4 inv = buildInv(position, rotation);
    Vect localPoint = inv.mult(worldPoint);
    for (auto* tp : triangles) {
        if (pointInTriangle(*tp, localPoint))
            return tp->getColor(localPoint);
    }
    return Color(1, 1, 1, 0);
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
