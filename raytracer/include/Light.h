#ifndef _Light_H
#define _Light_H

#include "Vect.h"
#include "Color.h"
#include "Source.h"
class Light : public Source {

    Vect position;
    Color color;
    double radius;

    public:

    // intensity > 0 enables inverse-square falloff; intensity is the brightness at distance 1
    double intensity;

    Light();
    Light(Vect, Color);
    Light(Vect, Color, double radius);

    //method functions

    virtual Vect getPosition();
    virtual Color getColor();
    virtual double getRadius();
    virtual double getIntensity() { return intensity; }
};

#endif