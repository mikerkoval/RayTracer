#include "Cone.h"
#include <iostream>
#include <cmath>
using namespace std;
double Cone::getRadius(){return radius;}
Color Cone::getColor(){return color;}
Color Cone::getColor(Vect p){return color;}


Vect Cone::getNormalAt(Vect point){
    Matrix4x4 t = Matrix4x4();
    t.translate(position.getX(), position.getY(), position.getZ());
    Matrix4x4 transform = t.mult(rotation);
    Matrix4x4 inv = transform.inverse();
    Matrix4x4 inv2 = rotation.inverse();
    Vect point2 = inv.mult(point);

    Matrix4x4 tinv2 = inv2.transpose();

    // apex is at z=max, base at z=min
    double apex_z = max;

    if(point2.getZ() > min - accuracy && point2.getZ() < min + accuracy){
        return tinv2.mult(Vect(0,0,-1)).normalize();
    }
    else {
        double dz = point2.getZ() - apex_z;
        Vect local_norm = Vect(point2.getX(), point2.getY(), -dz).normalize();
        Vect norm = tinv2.mult(local_norm);
        return norm.normalize();
    }
}

double Cone::findIntersection(Ray ray) {
    Matrix4x4 t = Matrix4x4();
    t.translate(position.getX(), position.getY(), position.getZ());
    Matrix4x4 transform = t.mult(rotation);
    Matrix4x4 inv = transform.inverse();

    Vect p0 = ray.getOrigin();
    Vect p1 = p0.add(ray.getDirection());
    p1 = inv.mult(p1);
    Vect new_ray_origin = inv.mult(p0);
    Vect new_ray_direction = p1.add(new_ray_origin.negative()).normalize();

    double rox = new_ray_origin.getX();
    double roy = new_ray_origin.getY();
    double roz = new_ray_origin.getZ();
    double rdx = new_ray_direction.getX();
    double rdy = new_ray_direction.getY();
    double rdz = new_ray_direction.getZ();

    // Cone equation: x^2 + y^2 = (z - apex_z)^2, apex at z=max
    double apex_z = max;
    double oz = roz - apex_z;

    double a = rdx*rdx + rdy*rdy - rdz*rdz;
    double b = 2*(rox*rdx + roy*rdy - oz*rdz);
    double c = rox*rox + roy*roy - oz*oz;

    double discriminant = b*b - 4*a*c;
    if (discriminant < 0) return -1;

    double t0 = (-b - sqrt(discriminant)) / (2*a);
    double t1 = (-b + sqrt(discriminant)) / (2*a);
    if (t0 > t1) { double tmp = t0; t0 = t1; t1 = tmp; }

    double z0 = roz + t0 * rdz;
    double z1 = roz + t1 * rdz;

    double time = -1;
    if(z0 > min && z0 < max && t0 >= 0.001){
        time = t0;
    }
    else if(z1 > min && z1 < max && t1 >= 0.001){
        time = t1;
    }

    // Base cap: disk at z=min, radius = |max - min| (cone radius at base)
    double cap_radius = fabs(max - min);
    if(fabs(rdz) > 1e-9){
        double t_cap = (min - roz) / rdz;
        if(t_cap >= 0.001){
            double cx = rox + t_cap * rdx;
            double cy = roy + t_cap * rdy;
            if(cx*cx + cy*cy <= cap_radius*cap_radius){
                if(time < 0.001 || t_cap < time)
                    time = t_cap;
            }
        }
    }

    if(time >= 0.001) return time;
    return -1;
}




Cone::Cone(){
    min = -1;
    max  = 0;
    radius = 1;
    color = Color(.5,.5,.5,.1);
    accuracy = .00001;
    rotation = Matrix4x4();
    position = Vect();
}
Cone::Cone(Color c){
    min = -1;
    max  = 0;
    radius = 1;
    color = c;
    accuracy = .00001;
    rotation = Matrix4x4();
    position = Vect();
}
Cone::Cone(double min0, double max0, Color c){
    min = min0;
    max = max0;
    radius = 1;
    color = c;
    accuracy = .00001;
    rotation = Matrix4x4();
    position = Vect();
}


