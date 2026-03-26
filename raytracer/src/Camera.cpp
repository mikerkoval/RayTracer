#include "Camera.h"
#include "math.h"

    
Vect Camera::getPosition(){ return campos;}
Vect Camera::getDirection(){ return camdir;}
Vect Camera::getRight(){ return camright;}
Vect Camera::getDown(){ return camdown;}

Camera::Camera(){
    campos = Vect(0,0,0);
    camdir = Vect(0,0,1);
    camright = Vect(0,0,0);
    camdown = Vect(0,0,0);
}

Camera::Camera(Vect pos, Vect dir,Vect right,Vect down){
    campos = pos;
    camdir = dir;
    camright = right;
    camdown = down;
}
