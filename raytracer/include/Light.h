#ifndef _Light_H
#define _Light_H

#include "Vect.h"
#include "Color.h"
#include "Source.h"
class Light : public Source {

    Vect position;
    Color color;

    public:

    Light();

    Light  (Vect, Color);

    //method functions

    virtual Vect getPosition();
    virtual Color getColor();
};

#endif