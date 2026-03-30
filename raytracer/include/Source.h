#ifndef _SOURCE_H
#define _SOURCE_H

#include "Color.h"
#include "Vect.h"
class Source {
    public:

    Source();
    virtual Vect getPosition();
    virtual Color getColor();
    virtual double getRadius() { return 0.0; }
    virtual double getIntensity() { return 0.0; }
};


#endif