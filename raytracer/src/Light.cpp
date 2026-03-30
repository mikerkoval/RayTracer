#include "Light.h"


Vect Light::getPosition(){ return position; }
Color Light::getColor(){ return color; }
double Light::getRadius(){ return radius; }

Light::Light(){
    position = Vect(0,0,0);
    color = Color(1,1,1,0);
    radius = 0.0;
    intensity = 0.0;
}

Light::Light(Vect p, Color c){
    position = p;
    color = c;
    radius = 0.0;
    intensity = 0.0;
}

Light::Light(Vect p, Color c, double r){
    position = p;
    color = c;
    radius = r;
    intensity = 0.0;
}
