#include "Plane.h"

Vect  Plane::getNormal(){return normal;}
double Plane::getDistance(){return distance;}
Color Plane::getColor(Vect p){return color;}
Vect Plane::getNormalAt(Vect point ){
    return normal;
}
double Plane::findIntersection(Ray ray) {
    Vect ray_direction = ray.getDirection();
    
    double a = ray_direction.dotProduct(normal);
    
    if (a <.0001 && a > -.0001) {
        // ray is parallel to the plane
        return -1;
    }
    else {
        double b = normal.dotProduct(ray.getOrigin().add(normal.mult(distance).negative()));
        if (1*b/a <.0001 && 1*b/a > -.0001){return -1;}
        return -1*b/a;
    }
}


Plane::Plane(){
    normal = Vect(1,0,0);
    distance  = 1;
    color = Color(.5,.5,.5,0);
   
}

Plane::Plane(Vect n, double  d, Color col){
    normal = n;
    distance = d; 
    color = col;
}
