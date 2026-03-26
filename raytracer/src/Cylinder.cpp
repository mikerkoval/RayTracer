#include "Cylinder.h"
#include <iostream>
using namespace std;
double Cylinder::getRadius(){return radius;}
Color Cylinder::getColor(){return color;}


Vect Cylinder::getNormalAt(Vect point){
    Matrix4x4 t = Matrix4x4();
    t.translate(position.getX(), position.getY(), position.getZ());
    Matrix4x4 transform = t.mult(rotation);
    Matrix4x4 inv = transform.inverse();
    Matrix4x4 inv2 = rotation.inverse();
    point = inv.mult(point);
   
    Matrix4x4 tinv = inv.transpose();
    Matrix4x4 tinv2 = inv2.transpose();

    if(point.getZ() > min - accuracy && point.getZ() < min+accuracy){
  
        return tinv2.mult(Vect(0,0,-1)).normalize();
    }
    if(point.getZ() > max - accuracy && point.getZ() < max+accuracy)
     {
        return tinv2.mult(Vect(0,0,1)).normalize();
    }

    else {
        Vect norm = tinv2.mult(Vect(point.getX(),point.getY(),0).normalize());
        return norm.normalize();
    }
}
double Cylinder::findIntersection(Ray ray) {
    Matrix4x4 t = Matrix4x4();
    t.translate(position.getX(), position.getY(), position.getZ());
    Matrix4x4 transform = t.mult(rotation);
    Matrix4x4 inv = transform.inverse();
  
   

    Vect p0 = ray.getOrigin();
    Vect p1 = p0.add( ray.getDirection());
    p1 = inv.mult(p1);
    Vect ray_origin = ray.getOrigin();
    Vect new_ray_origin = inv.mult(p0);
    
    double rox = new_ray_origin.getX();
    double roy = new_ray_origin.getY();
    double roz = new_ray_origin.getZ();
    
    Vect ray_direction = ray.getDirection();
    Vect new_ray_direction = p1.add(new_ray_origin.negative()).normalize();
   
    
    double rdx = new_ray_direction.getX();
    double rdy = new_ray_direction.getY();
    double rdz = new_ray_direction.getZ();

    double a = pow(rdx, 2) + pow(rdy, 2);
    double b = 2 * rox * rdx + 2* roy * rdy;
    double c = pow(rox, 2) + pow(roy, 2) - radius;
    

    double discriminant = b*b - 4*a*c;
    
    if (discriminant < 0) {
        /// the ray intersects the cylinder
        return -1   ;
    }

    double t0 = ((-1*b - sqrt(discriminant))/(2*a)) ;
    double t1 = ((sqrt(discriminant) - b)/(2*a)) ;
    if (t0>t1) {double tmp = t0;t0=t1;t1=tmp;}
    double z0 = roz + t0 * rdz;
    double z1 = roz + t1 * rdz;

    ////
    double time = -1;
    if (z0<min)
    {
        if (z1<min)
            return false;
        else
        {
            // hit the cap
            float th = (min - roz ) / rdz;
            if (th<=0) return false;
        
            time =  th;
        }
    }
    else if (z0>=min && z0<=max)
    {
        // hit the cylinder bit
        if (t0<=0) return -1;
            
        time =  t0;
    }
    else if (z0>max)
    {
        if (z1>max)
            return false;
        else
        {
            // hit the cap
            float th = (max - roz ) / rdz;
            if (th<=0) return false;

       
            time =  th;
        }
    }
    ///
    if(time >=.001){
        
        Vect point = new_ray_origin.add(new_ray_direction.mult(time));
        //cout << time << endl;
        point = transform.mult(point);
        //cout << (point.getx() - ray_origin.getX())/ ray_direction.getX() << endl;

        return (point.getX() - ray_origin.getX())/ ray_direction.getX();
    }
   
    
    //
    return -1;
    
}




Cylinder::Cylinder(){
    min = -1;
    max  = 1;
    radius = 1;
    color = Color(.5,.5,.5,.1);
    accuracy = .00001;
    rotation = Matrix4x4();
    position = Vect();
}
Cylinder::Cylinder(Color c){
    min = -1;
    max  = 1;
    radius = 1;
    color = c;
    accuracy = .00001;
    rotation = Matrix4x4();
    position = Vect();
}

Cylinder::Cylinder(double min0, double  max0, double radius0, Color c){
    min = min0;
    max  = max0;
    radius = radius0;
    color = c;
    rotation = Matrix4x4();
    position = Vect();
    accuracy = .00001;
}

