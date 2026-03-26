#ifndef _MATRIX_H
#define _MATRIX_H

#include "math.h"
#include "Vect.h"
class Matrix {
    double x1, y1, z1;
    double x2, y2, z2;
    double x3, y3, z3;
    public:
    
    Matrix ();
    Matrix (Vect, double);
    Matrix (double, double, double, double, double, double, double, double, double);
    
    // method functions
    double getX1();
    double getX2();
    double getX3();
    double getY1();
    double getY2();
    double getY3();
    double getZ1();
    double getZ2();
    double getZ3();
    
    

    Vect vectMult (Vect v);
    
};



#endif
