#ifndef _TRIANGLEMESH_H
#define _TRIANGLEMESH_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include "Triangle.h"
#include "Ray.h"
#include "Matrix.h"
#include <vector>
#include "Sphere.h"
#include "Magick++.h"
class TriangleMesh: public Object{
    Vect center;
    Color color;

    vector<Vect> corners;
    vector<Vect> normals;
    vector<Vect> textures;
    vector<Triangle*> triangles;
    vector<Triangle> triangleOs;
    Sphere boundingSphere;
    Magick::Image* texture;
    bool setText;
    bool clearLight;
    public:

    TriangleMesh();
    TriangleMesh(string, Color);
    TriangleMesh(string, Color,  bool);
    TriangleMesh(string, Magick::Image*);
    TriangleMesh(string, Magick::Image*, bool);


    //method functions
    bool pointInTriangle(Triangle t, Vect p);
    
    void createMesh(string c);
    Color getColor(Vect p);
    virtual bool getCL();
    virtual Vect getNormalAt(Vect point);
    virtual double findIntersection(Ray ray) ;
    virtual void rotate(Matrix r);
    virtual void translate(Vect p);
    virtual void scale(double x, double y, double z);

};

#endif