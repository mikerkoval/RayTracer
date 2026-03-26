#ifndef _Cylinder_H
#define _Cylinder_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include "Matrix4x4.h"
class Cylinder: public Object{

    double min;
    double max;
    double radius;
    double accuracy;

    Color color;

    public:

    Cylinder();
    Cylinder(Color);
    

    Cylinder  (double, double, double, Color);

    //method functions

    Vect  getCenter();
    double getRadius();
    Color getColor();

    virtual Vect getNormalAt(Vect point);
    virtual double findIntersection(Ray ray) ;
    

};
#endif