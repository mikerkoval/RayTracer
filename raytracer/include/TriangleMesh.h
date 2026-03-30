#ifndef _TRIANGLEMESH_H
#define _TRIANGLEMESH_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include "Triangle.h"
#include "Ray.h"
#include "Matrix.h"
#include "Matrix4x4.h"
#include "Texture.h"
#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include "Sphere.h"
#include "Magick++.h"

struct MtlMaterial {
    Color kd { 0.8, 0.8, 0.8, 0 };
    double ks  { 0.5 };
    double ns  { 0.0 };
    double ni  { 1.0 };
    double d   { 1.0 };
    std::unique_ptr<ImageTexture> map_kd;
};

struct AABB {
    Vect min, max;
    AABB() : min(Vect(1e18,1e18,1e18)), max(Vect(-1e18,-1e18,-1e18)) {}
    AABB(Vect mn, Vect mx) : min(mn), max(mx) {}
    void expand(const AABB& o);
    double intersect(const Ray& ray) const; // returns t or -1
};

struct BVHNode {
    AABB bounds;
    std::unique_ptr<BVHNode> left, right;
    std::vector<Triangle*> tris; // non-empty only in leaves
    bool isLeaf() const { return !left; }
};

class TriangleMesh: public Object{
    Vect center;
    Color color;

    // local-space data loaded from OBJ (never mutated)
    vector<Vect> corners;
    vector<Vect> vnormals;
    vector<Vect> textures;
    vector<Triangle> triangleOs;
    vector<Triangle*> triangles;

    // MTL materials and per-triangle material index + UVs
    vector<MtlMaterial> materials;
    vector<int> triMaterial;              // parallel to triangleOs
    vector<array<Vect,3>> triUVs;        // UV coords per triangle vertex

    double boundingRadius;

    std::unique_ptr<BVHNode> bvhRoot;
    static thread_local Triangle* lastHitTriangle;

    Magick::Image* texture;
    bool setText;
    bool clearLight;

    void createMesh(string c);
    void loadMtl(const string& path, const string& baseDir);
    void rebuildBVH();
    bool pointInTriangle(Triangle& t, Vect p);

    AABB triangleAABB(Triangle* t) const;
    std::unique_ptr<BVHNode> buildBVH(std::vector<Triangle*>& tris, int start, int end);
    double traverseBVH(const BVHNode* node, const Ray& localRay, Triangle*& hitTri) const;

public:
    TriangleMesh();
    TriangleMesh(string, Color);
    TriangleMesh(string, Color, bool);
    TriangleMesh(string, Magick::Image*);
    TriangleMesh(string, Magick::Image*, bool);

    Color getColor(Vect p);
    virtual bool getCL();
    virtual Vect getNormalAt(Vect point);
    virtual double findIntersection(Ray ray);
};

#endif
