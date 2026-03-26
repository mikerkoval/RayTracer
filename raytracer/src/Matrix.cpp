#include "Matrix.h"

#include "math.h"


    // method functions
double Matrix::getX1() { return x1; }
double Matrix::getX2() { return x2; }
double Matrix::getX3() { return x3; }
double Matrix::getY1() { return y1; }
double Matrix::getY2() { return y2; }
double Matrix::getY3() { return y3; }
double Matrix::getZ1() { return z1; }
double Matrix::getZ2() { return z2; }
double Matrix::getZ3() { return z3; }
    
    

Vect Matrix::vectMult (Vect v) {
    double vx = v.getX();
    double vy = v.getY();
    double vz = v.getZ();
    double x = (x1 * vx + y1 * vy + z1 * vz);
    double y = (x2 * vx + y2 * vy + z2 * vz);
    double z = (x3 * vx + y3 * vy + z3 * vz);
    return Vect (x, y, z);
}


Matrix::Matrix () {
    x1 = 1;
    y1 = 0;
    z1 = 0;

    x2 = 0;
    y2 = 1;
    z2 = 0;

    x3 = 0;
    y3 = 0;
    z3 = 1;

}
Matrix::Matrix(Vect v, double t){
    v = v.normalize();
    double x = v.getX();
    double y = v.getY();
    double z = v.getZ();
    x1 = cos(t) + x*x*(1-cos(t));
    x2 = y*x*(1-cos(t)) + z*sin(t);
    x3 = z*x*(1-cos(t)) - y*sin(t);

    y1 = x*y*(1-cos(t)) - z*sin(t);
    y2 = cos(t) + y*y*(1-cos(t));
    y3 = z*y*(1-cos(t)) + x*sin(t);

    z1 = x*z*(1-cos(t)) + y*sin(t);
    z2 = y*z*(1-cos(t)) - x*sin(t);
    z3 = cos(t) + z*z*(1-cos(t));
}
Matrix::Matrix (double a, double b, double c, double d, double e, double f, double g, double h, double i) {
    x1 = a;
    y1 = b;
    z1 = c;

    x2 = d;
    y2 = e;
    z2 = f;

    x3 = g;
    y3 = h;
    z3 = i;
}


