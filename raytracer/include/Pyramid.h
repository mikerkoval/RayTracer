#ifndef _PYRAMID_H
#define _PYRAMID_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include "Triangle.h"
#include "Ray.h"
#include "Matrix.h"
#include <vector>

class Pyramid: public Object{
    Vect center;
    double sides;
    double radius;
    double height;
    Color color;

    vector<Vect> corners;
    vector<Triangle*> triangles;
    vector<Triangle> triangleOs;

    public:

    Pyramid();

    Pyramid  (Vect, double, double, double, Color);

    //method functions
    bool pointInTriangle(Triangle t, Vect p);
    
    void createPyramid();
    Color getColor();

    virtual Vect getNormalAt(Vect point);
    virtual double findIntersection(Ray ray) ;
    virtual void rotate(Matrix r);
    

};

#endif