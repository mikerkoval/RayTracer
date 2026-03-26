#ifndef _PLANE_H
#define _PLANE_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
class Plane: public Object{

    Vect normal;
    double distance;
    Color color;

    public:

    Plane();

    Plane  (Vect, double,  Color);

    //method functions

    Vect  getNormal();
    double getDistance();
    virtual Color getColor(Vect p);
    virtual Vect getNormalAt(Vect point );
    virtual double findIntersection(Ray ray);
};

#endif