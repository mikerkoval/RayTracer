#ifndef _MATRIX4X4_H
#define _MATRIX4X4_H

#include "math.h"
#include <iostream>
#include <vector>
#include <Vect.h>
#include "Matrix.h"
class Matrix4x4 {
    double m00, m01, m02, m03;
    double m10, m11, m12, m13;
    double m20, m21, m22, m23;
    double m30, m31, m32, m33;
    
    public:
    
    Matrix4x4 ();
    
    Matrix4x4 (Matrix m) ;
    
    // method functions
    double get00(); double get01(); double get02(); double get03();
    double get10(); double get11(); double get12(); double get13();
    double get20(); double get21(); double get22(); double get23();   
    double get30(); double get31(); double get32(); double get33();

    void set00(double n);  void set01(double n);  void set02(double n);  void set03(double n);
    void set10(double n);  void set11(double n);  void set12(double n);  void set13(double n);
    void set20(double n);  void set21(double n);  void set22(double n);  void set23(double n);   
    void set30(double n);  void set31(double n);  void set32(double n);  void set33(double n);

    
    
    Matrix4x4 add (Matrix4x4 m);

    Matrix4x4 mult (double scalar);
    Matrix4x4 mult (Matrix4x4 m);
    void translate (double x, double y, double z);
    void rotateY(double a);
    void rotateX(double a);
    void rotateZ(double a);
    Vect mult (Vect v);
    void print ();
    Matrix4x4 inverse ();
    Matrix4x4 transpose ();
};


#endif
