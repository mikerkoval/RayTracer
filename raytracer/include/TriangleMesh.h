#ifndef _TRIANGLEMESH_H
#define _TRIANGLEMESH_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include "Triangle.h"
#include "Ray.h"
#include "Matrix.h"
#include "Matrix4x4.h"
#include <vector>
#include "Sphere.h"
#include "Magick++.h"

class TriangleMesh: public Object{
    Vect center;
    Color color;

    // local-space data loaded from OBJ (never mutated)
    vector<Vect> corners;
    vector<Vect> vnormals;
    vector<Vect> textures;
    vector<Triangle> triangleOs;
    vector<Triangle*> triangles;

    double boundingRadius;  // radius of bounding sphere in local space

    Magick::Image* texture;
    bool setText;
    bool clearLight;

    void createMesh(string c);
    bool pointInTriangle(Triangle& t, Vect p);

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
