#include "Matrix4x4.h"
#include "math.h"
#include <iostream>
#include <iomanip>
#include <Eigen/Dense>

using Eigen::MatrixXd;


Matrix4x4::Matrix4x4 () {
    m00 = 1; m01 = 0; m02 = 0; m03 = 0; 
    m10 = 0; m11 = 1; m12 = 0; m13 = 0;
    m20 = 0; m21 = 0; m22 = 1; m23 = 0;
    m30 = 0; m31 = 0; m32 = 0; m33 = 1;
}

Matrix4x4::Matrix4x4 (Matrix m) {
    m00 = m.getX1(); m01 = m.getX2(); m02 = m.getX3(); m03 = 0;
    m10 = m.getY1(); m11 = m.getY2(); m12 = m.getY3(); m13 = 0;
    m20 = m.getZ1(); m21 = m.getZ2(); m22 = m.getZ3(); m23 = 0;
    m30 = 0;         m31 = 0;         m32 = 0;         m33 = 1;
}
Vect Matrix4x4::mult (Vect v){
    double a = m00 * v.getX() + m01 * v.getY() + m02 * v.getZ() + m03 * 1;
    double b = m10 * v.getX() + m11 * v.getY() + m12 * v.getZ() + m13 * 1;
    double c = m20 * v.getX() + m21 * v.getY() + m22 * v.getZ() + m23 * 1;
    double d = m30 * v.getX() + m31 * v.getY() + m32 * v.getZ() + m33 * 1;
    return Vect(a/d, b/d, c/d);
}
Matrix4x4 Matrix4x4::mult (Matrix4x4 n){
    double n00 = n.get00(); double n01 = n.get01(); double n02 = n.get02(); double n03 = n.get03();
    double n10 = n.get10(); double n11 = n.get11(); double n12 = n.get12(); double n13 = n.get13();
    double n20 = n.get20(); double n21 = n.get21(); double n22 = n.get22(); double n23 = n.get23();
    double n30 = n.get30(); double n31 = n.get31(); double n32 = n.get32(); double n33 = n.get33(); 

    Matrix4x4 m = Matrix4x4();
    m.set00((m00 * n00) + (m01 * n10) + (m02 * n20) + (m03 * n30));
    m.set01((m00 * n01) + (m01 * n11) + (m02 * n21) + (m03 * n31));
    m.set02((m00 * n02) + (m01 * n12) + (m02 * n22) + (m03 * n32));
    m.set03((m00 * n03) + (m01 * n13) + (m02 * n23) + (m03 * n33));

    m.set10((m10 * n00) + (m11 * n10) + (m12 * n20) + (m13 * n30));
    m.set11((m10 * n01) + (m11 * n11) + (m12 * n21) + (m13 * n31));
    m.set12((m10 * n02) + (m11 * n12) + (m12 * n22) + (m13 * n32));
    m.set13((m10 * n03) + (m11 * n13) + (m12 * n23) + (m13 * n33));

    m.set20((m20 * n00) + (m21 * n10) + (m22 * n20) + (m23 * n30));
    m.set21((m20 * n01) + (m21 * n11) + (m22 * n21) + (m23 * n31));
    m.set22((m20 * n02) + (m21 * n12) + (m22 * n22) + (m23 * n32));
    m.set23((m20 * n03) + (m21 * n13) + (m22 * n23) + (m23 * n33));

    m.set30((m30 * n00) + (m31 * n10) + (m32 * n20) + (m33 * n30));
    m.set31((m30 * n01) + (m31 * n11) + (m32 * n21) + (m33 * n31));
    m.set32((m30 * n02) + (m31 * n12) + (m32 * n22) + (m33 * n32));
    m.set33((m30 * n03) + (m31 * n13) + (m32 * n23) + (m33 * n33));
    return m;
}

void Matrix4x4::translate (double x, double y, double z){
    m03 = m03 + x;
    m13 = m13 + y;
    m23 = m23 + z;
    
}
void Matrix4x4::rotateY (double a){
    m00 = cos(a);
    m02 = sin(a);
    m20 = -sin(a);
    m22 = cos(a);
    
}
void Matrix4x4::rotateX (double a){
    m11 = cos(a);
    m12 = -sin(a);
    m21 = sin(a);
    m22 = cos(a);
    
}
void Matrix4x4::rotateZ (double a){
    m00 = cos(a);
    m01 = -sin(a);
    m10 = sin(a);
    m11 = cos(a);
    
}
//get
double Matrix4x4::get00(){return m00;}
double Matrix4x4::get01(){return m01;}
double Matrix4x4::get02(){return m02;}
double Matrix4x4::get03(){return m03;}

double Matrix4x4::get10(){return m10;} 
double Matrix4x4::get11(){return m11;}
double Matrix4x4::get12(){return m12;}
double Matrix4x4::get13(){return m13;}

double Matrix4x4::get20(){return m20;}
double Matrix4x4::get21(){return m21;}
double Matrix4x4::get22(){return m22;}
double Matrix4x4::get23(){return m23;}  
    
double Matrix4x4::get30(){return m30;}
double Matrix4x4::get31(){return m31;}
double Matrix4x4::get32(){return m32;}
double Matrix4x4::get33(){return m33;}

//set
void Matrix4x4::set00(double s){ m00 = s;}
void Matrix4x4::set01(double s){ m01 = s;}
void Matrix4x4::set02(double s){ m02 = s;}
void Matrix4x4::set03(double s){ m03 = s;}

void Matrix4x4::set10(double s){ m10 = s;} 
void Matrix4x4::set11(double s){ m11 = s;}
void Matrix4x4::set12(double s){ m12 = s;}
void Matrix4x4::set13(double s){ m13 = s;}

void Matrix4x4::set20(double s){ m20 = s;}
void Matrix4x4::set21(double s){ m21 = s;}
void Matrix4x4::set22(double s){ m22 = s;}
void Matrix4x4::set23(double s){ m23 = s;}  
    
void Matrix4x4::set30(double s){ m30 = s;}
void Matrix4x4::set31(double s){ m31 = s;}
void Matrix4x4::set32(double s){ m32 = s;}
void Matrix4x4::set33(double s){ m33 = s;}
//print
void Matrix4x4::print(){
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    std::cout << "| "  << m00 << " " << m01 << " " << m02 << " " << m03 << " |" << std::endl; 
    std::cout << "| "  << m10 << " " << m11 << " " << m12 << " " << m13 << " |" << std::endl; 
    std::cout << "| "  << m20 << " " << m21 << " " << m22 << " " << m23 << " |" << std::endl; 
    std::cout << "| "  << m30 << " " << m31 << " " << m32 << " " << m33 << " |" << std::endl; 
}
Matrix4x4 Matrix4x4::inverse(){
    MatrixXd m(4,4);
    m(0,0) = m00;  m(0,1) = m01;  m(0,2) = m02;  m(0,3) = m03;
    m(1,0) = m10;  m(1,1) = m11;  m(1,2) = m12;  m(1,3) = m13;
    m(2,0) = m20;  m(2,1) = m21;  m(2,2) = m22;  m(2,3) = m23;
    m(3,0) = m30;  m(3,1) = m31;  m(3,2) = m32;  m(3,3) = m33;
    m = m.inverse();
    Matrix4x4 f = Matrix4x4();
    f.set00(m(0,0));
    f.set01(m(0,1));
    f.set02(m(0,2));
    f.set03(m(0,3));

    f.set10(m(1,0));
    f.set11(m(1,1));
    f.set12(m(1,2));
    f.set13(m(1,3));

    f.set20(m(2,0));
    f.set21(m(2,1));
    f.set22(m(2,2));
    f.set23(m(2,3));

    f.set30(m(3,0));
    f.set31(m(3,1));
    f.set32(m(3,2));
    f.set33(m(3,3));
    return f;
}
Matrix4x4 Matrix4x4::transpose(){
    MatrixXd m(4,4);
    m(0,0) = m00;  m(0,1) = m01;  m(0,2) = m02;  m(0,3) = m03;
    m(1,0) = m10;  m(1,1) = m11;  m(1,2) = m12;  m(1,3) = m13;
    m(2,0) = m20;  m(2,1) = m21;  m(2,2) = m22;  m(2,3) = m23;
    m(3,0) = m30;  m(3,1) = m31;  m(3,2) = m32;  m(3,3) = m33;
    m.transposeInPlace();
    Matrix4x4 f = Matrix4x4();
    f.set00(m(0,0));
    f.set01(m(0,1));
    f.set02(m(0,2));
    f.set03(m(0,3));

    f.set10(m(1,0));
    f.set11(m(1,1));
    f.set12(m(1,2));
    f.set13(m(1,3));

    f.set20(m(2,0));
    f.set21(m(2,1));
    f.set22(m(2,2));
    f.set23(m(2,3));

    f.set30(m(3,0));
    f.set31(m(3,1));
    f.set32(m(3,2));
    f.set33(m(3,3));
    return f;
}
