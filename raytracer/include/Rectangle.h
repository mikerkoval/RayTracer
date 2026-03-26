#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include "Triangle.h"
#include "Ray.h"
#include "Matrix.h"
#include "Raytracer.h"
using namespace std;
class Rectangle: public Object{

    Vect center;
    Color color;
    double length;
    double width;
    double height;
    vector<Vect> corners;
    vector<Triangle*> triangles;

    Vect c1;
    Vect c2;
    Vect c3;
    Vect c4;
    Vect c5;
    Vect c6;
    Vect c7;
    Vect c8;

    Triangle t1;
    Triangle t2;
    Triangle t3;
    Triangle t4;
    Triangle t5;
    Triangle t6;
    Triangle t7;
    Triangle t8;
    Triangle t9;
    Triangle t10;
    Triangle t11;
    Triangle t12;
    public:

    Rectangle();

    Rectangle  (Vect, double, double, double, Color);

    //method functions
    bool pointInTriangle(Triangle t, Vect p);
    void createRectangle();
    Color getColor();

    virtual int move(Vect m);
    virtual Vect getNormalAt(Vect point);
    virtual double findIntersection(Ray ray);
    virtual void rotate(Matrix r);
    virtual void rotateY(double s);
    virtual void rotateX(double s);
    

};
#endif