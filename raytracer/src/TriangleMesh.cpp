#include "TriangleMesh.h"
#include "Raytracer.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
using namespace std;

// ---- string helpers ----
static vector<string> split(const string& s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) elems.push_back(item);
    return elems;
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

    // bounding radius in local space
    boundingRadius = 0;
    for (auto& v : corners) {
        double m = v.magnitude();
        if (m > boundingRadius) boundingRadius = m;
    }
}

// ---- point-in-triangle (local space) ----
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

    // normalise barycentric
    double len = sqrt(u*u + v*v + w*w);
    if (len > 1e-10) { u /= len; v /= len; w /= len; }

    double eps = 1e-13;
    return (u >= -eps && u <= 1+eps &&
            v >= -eps && v <= 1+eps &&
            w >= -eps && w <= 1+eps &&
            onPlane > -eps && onPlane < eps);
}

// ---- transform helpers ----
// Build (T * R)^-1 for ray transform
static Matrix4x4 buildInv(Vect pos, Matrix4x4 rot) {
    Matrix4x4 t;
    t.translate(pos.getX(), pos.getY(), pos.getZ());
    return t.mult(rot).inverse();
}

// ---- findIntersection ----
double TriangleMesh::findIntersection(Ray ray) {
    Matrix4x4 inv = buildInv(position, rotation);

    // Transform ray into local space
    Vect p0 = ray.getOrigin();
    Vect p1 = p0.add(ray.getDirection());
    Vect lp0 = inv.mult(p0);
    Vect lp1 = inv.mult(p1);
    Vect ldir = lp1.add(lp0.negative()).normalize();
    Ray localRay(lp0, ldir);

    // Bounding sphere test in local space
    Sphere bsphere(Vect(0,0,0), boundingRadius, Color());
    if (bsphere.findIntersection(localRay) == -1) return -1;

    double best = -1;
    for (auto* tp : triangles) {
        double t = tp->findIntersection(localRay);
        if (t > 0.01 && (best < 0 || t < best))
            best = t;
    }
    return best;
}

// ---- getNormalAt ----
Vect TriangleMesh::getNormalAt(Vect worldPoint) {
    Matrix4x4 inv = buildInv(position, rotation);
    Vect localPoint = inv.mult(worldPoint);

    Matrix4x4 rotInv = rotation.inverse();
    Matrix4x4 tinv = rotInv.transpose();

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
    Matrix4x4 inv = buildInv(position, rotation);
    Vect localPoint = inv.mult(worldPoint);

    for (auto* tp : triangles) {
        if (pointInTriangle(*tp, localPoint))
            return tp->getColor(localPoint);
    }
    return Color(1, 1, 1, 0);
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
