#ifndef _OBJECT_H
#define _OBJECT_H

#include "Ray.h"
#include "Vect.h"
#include "Color.h"
#include "Matrix.h"
#include "Matrix4x4.h"
#include "Texture.h"
class Object {

    public:
    Matrix4x4 rotation;
    Matrix4x4 inverse;
    Vect position;
    bool clearLight;
    Texture* texture;
    NormalMapTexture* normalMap;
    double shininess;     // Phong exponent, 0 = no specular highlight
    double ior;           // index of refraction, 0 = opaque
    double transparency;  // 0 = opaque, 1 = fully transparent
    Color emission;       // emissive color, black = no emission
    Object();

    //method functions
    virtual bool getCL();
    virtual bool isConvex() { return false; }
    virtual Color getColor(Vect p);
    virtual int move(Vect);
    virtual double findIntersection(Ray ray);
    virtual Vect getNormalAt(Vect intersection_position);
    virtual void rotate(Matrix r);
    virtual void rotateY(double s);
    virtual void rotateX(double s);
    virtual void rotateZ(double s);
    virtual void translate(Vect v);
    virtual void scale(double x, double y, double z);
};



#endif