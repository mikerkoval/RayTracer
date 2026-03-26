#ifndef _SOURCE_H
#define _SOURCE_H

#include "Color.h"
#include "Vect.h"
class Source {
    public:

    Source();
    virtual Vect getPosition();
    virtual Color getColor();
};


#endif