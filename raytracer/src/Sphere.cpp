#include "Sphere.h"
#include <iostream>

using namespace std;

Vect Sphere::getCenter(){return center;}
double Sphere::getRadius(){return radius;}

void Sphere::getUV(Vect worldPoint, double& u, double& v) {
    Vect local = inverse.mult(worldPoint.add(center.negative())).normalize();
    double theta   = acos(max(-1.0, min(1.0, local.getY())));
    double epsilon = atan2(local.getZ(), local.getX()) + M_PI;
    u = epsilon / (2.0 * M_PI);
    v = theta  / M_PI;
}

Color Sphere::getColor(Vect p){
    if (Object::texture) {
        double u, v;
        getUV(p, u, v);
        return Object::texture->getColor(u, v);
    }
    if(texture){
         p = inverse.mult(p);
         p = p.normalize();
        double theta = acos(p.getY()/p.magnitude());
        theta = theta/3.1415926;
   
        double epsilon = atan2(p.getZ(),p.getX());
        epsilon = epsilon + 3.1415926;
        epsilon = epsilon /3.1415926/2;


        int w = texture ->columns();
        int h = texture ->rows();

        double x = w * epsilon ; x = (int) x;
        double y = h * theta; y = (int) y;
        // cout << x << " " << y << endl;
        //vector<unsigned int> c = texture -> getPixel(x,y); 
        
        //return Color(c[0]/255.0,c[1]/255.0,c[2]/255.0,0);

        int row = y;
        int column = x;
        
        Magick::PixelPacket *pixels = texture->getPixels(0, 0, w, h);
        Magick::Color color = pixels[w * row + column];

        double range = pow(2, texture -> modulusDepth());

        double r = color.redQuantum()/range ;
        double g = color.greenQuantum()/range ;
        double b = color.blueQuantum()/range ;
        double s = 0;
        if(r!= 0 && g!= 0 && b != 0){ s = .3;}
        return Color(r, g, b, s);

       

    }
    return color;
}
int Sphere::move(Vect m){
    center = m;
    return 1;

}
Vect Sphere::getNormalAt(Vect point){
    Vect normal_vect = point.add(center.negative()).normalize();
    return normal_vect;
}
double Sphere::findIntersection(Ray ray) {
    Vect ray_origin = ray.getOrigin();
    double ray_origin_x = ray_origin.getX();
    double ray_origin_y = ray_origin.getY();
    double ray_origin_z = ray_origin.getZ();
    
    Vect ray_direction = ray.getDirection();
    double ray_direction_x = ray_direction.getX();
    double ray_direction_y = ray_direction.getY();
    double ray_direction_z = ray_direction.getZ();
    
    Vect sphere_center = center;
    double sphere_center_x = sphere_center.getX();
    double sphere_center_y = sphere_center.getY();
    double sphere_center_z = sphere_center.getZ();
    
    double a = 1; // normalized
    double b = (2*(ray_origin_x - sphere_center_x)*ray_direction_x) + (2*(ray_origin_y - sphere_center_y)*ray_direction_y) + (2*(ray_origin_z - sphere_center_z)*ray_direction_z);
    double c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_y - sphere_center_y, 2) + pow(ray_origin_z - sphere_center_z, 2) - (radius*radius);
    
    double discriminant = b*b - 4*c;
    
    if (discriminant > 0) {
        /// the ray intersects the sphere
        
        // the first root
        double root_1 = ((-1*b - sqrt(discriminant))/2) - 0.000001;
        
        if (root_1 > 0) {
            // the first root is the smallest positive root
            return root_1;
        }
        else {
            // the second root is the smallest positive root
            double root_2 = ((sqrt(discriminant) - b)/2) - 0.000001;
            return root_2;
        }
    }
    else {
        // the ray missed the sphere
        return -1;
    }
}

void Sphere::translate(Vect v){
    center = center.add(v);
}
void Sphere::rotate(Matrix r){
    center = r.vectMult(center);
}
void Sphere::rotateY(double s){
    Matrix4x4 m = Matrix4x4();
    m.rotateY(s);
    rotation = m.mult(rotation);
    inverse = rotation.inverse();
}

void Sphere::rotateX(double s){
    Matrix4x4 m = Matrix4x4();
    m.rotateX(s);
    rotation = m.mult(rotation);
    inverse = rotation.inverse();
}
void Sphere::rotateZ(double s){
    Matrix4x4 m = Matrix4x4();
    m.rotateZ(s);
    rotation = m.mult(rotation);
    inverse = rotation.inverse();
}
void Sphere::scale(double s){
    radius = radius * s;
}

Sphere::Sphere(){
    center = Vect(0,0,0);
    radius  = 1;
    color = Color(.5,.5,.5,0);
    texture= NULL;
}

Sphere::Sphere(Vect c, double  r, Magick::Image* map){
    center = c;
    radius = r; 
    texture = map;
    
}

Sphere::Sphere(Vect c, double  r, Color col){
    center = c;
    radius = r; 
    color = col;
   texture= NULL;
    
}

