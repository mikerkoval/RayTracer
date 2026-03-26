#include "Object.h"
#include "Matrix4x4.h"

Color Object::getColor(Vect point){return Color(0,0,0,0);}
int Object::move(Vect){return 0;}
double Object::findIntersection(Ray ray){return 0;}
bool Object::getCL(){return clearLight;}
Vect Object::getNormalAt(Vect intersection_position){
    return Vect(0,0,0);
}
 void Object::rotate(Matrix r){}
void Object::rotateY(double s){
    Matrix4x4 m = Matrix4x4();
    m.rotateY(s);
    rotation = m.mult(rotation);
    inverse = rotation.inverse();
}
void Object::rotateX(double s){
    Matrix4x4 m = Matrix4x4();
    m.rotateX(s);
    rotation = m.mult(rotation);
    inverse = rotation.inverse();
}
void Object::rotateZ(double s){
    Matrix4x4 m = Matrix4x4();
    m.rotateZ(s);
    rotation = m.mult(rotation);
    inverse = rotation.inverse();
}
void Object::translate(Vect v){
    position = position.add(v);
}
void Object::scale(double x, double y, double z){
    Matrix4x4 m = Matrix4x4();
    m.set00(x);
    m.set11(y);
    m.set22(z);
    rotation = m.mult(rotation);
}

Object::Object(){
   
}

