#ifndef _Cone_H
#define _Cone_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include "Matrix4x4.h"
class Cone: public Object{

    double min;
    double max;
    double radius;
    double accuracy;

    Color color;

    public:

    Cone();
    Cone(Color c);


    //method functions

    Vect  getCenter();
    double getRadius();
    Color getColor();

    virtual Vect getNormalAt(Vect point);
    virtual double findIntersection(Ray ray) ;
    

};
#endif