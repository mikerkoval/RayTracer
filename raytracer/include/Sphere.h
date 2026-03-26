#ifndef _SPHERE_H
#define _SPHERE_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include "Matrix.h"
#include "bmp.h"
#include "Magick++.h"
class Sphere: public Object{

    Vect center;
    double radius;
    Color color;
    Magick::Image* texture;
    
    public:

    Sphere();

    Sphere  (Vect, double,  Color);
    Sphere  (Vect, double,  Magick::Image*);
    //method functions

    Vect  getCenter();
    double getRadius();
    Color getColor(Vect p);
    void scale(double);
    virtual int move(Vect m);
    virtual Vect getNormalAt(Vect point);
    virtual double findIntersection(Ray ray) ;
    virtual void translate(Vect v);
    virtual void rotate(Matrix r);

    virtual void rotateY(double s) ;
    virtual void rotateX(double s) ;
    virtual void rotateZ(double s) ;
    

};
#endif