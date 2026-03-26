#include "Light.h"


 Vect Light::getPosition(){return position;}
 Color Light::getColor(){return color;}

Light::Light(){
    position = Vect(0,0,0);
    color = Color(1,1,1,0);
   
}

Light::Light(Vect p, Color c){
    position = p;
    color = c;
}
