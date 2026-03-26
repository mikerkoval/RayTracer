#ifndef _CAMERA_H
#define _CAMERA_H
#include "Vect.h"
class Camera {

    Vect campos, camdir, camright, camdown;
    public:

    Camera();

    Camera (Vect, Vect, Vect, Vect);

    //method functions

    
    Vect getPosition();
    Vect getDirection();
    Vect getRight();
    Vect getDown();
};

#endif