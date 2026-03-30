#include "Pyramid.h"
#include "Raytracer.h"
#include "Matrix4x4.h"
#include <iostream>
using namespace std;

thread_local int Pyramid::last_hit_index = 0;
bool Pyramid::pointInTriangle(Triangle t, Vect p){
    Vect normal = t.getNormalAt(p);
    Vect A = t.getA();
    Vect B = t.getB();
    Vect C = t.getC();
    Vect Q = p;
    Vect CA (C.getX()- A.getX(),C.getY()- A.getY(), C.getZ()- A.getZ()); 
    Vect QA (Q.getX()- A.getX(),Q.getY()- A.getY(), Q.getZ()- A.getZ()); 
    double test1 = (CA.crossProduct(QA)).dotProduct(normal);

    Vect BC (B.getX()- C.getX(),B.getY()- C.getY(), B.getZ()- C.getZ()); 
    Vect QC (Q.getX()- C.getX(),Q.getY()- C.getY(), Q.getZ()- C.getZ()); 
    double test2 = (BC.crossProduct(QC)).dotProduct(normal);

    Vect AB (A.getX()- B.getX(), A.getY()- B.getY(), A.getZ()- B.getZ()); 
    Vect QB (Q.getX()- B.getX(),Q.getY()- B.getY(), Q.getZ()- B.getZ()); 
    double test3 = (AB.crossProduct(QB)).dotProduct(normal);

    double test4 = QA.dotProduct(t.getNormalAt(p));
  

    if(test1 >= 0 && test2 >= 0 && test3 >= 0 && test4 < .00001 && test4 > -.00001){

        return true;
    }
    else{
        return false;
    }
}
void Pyramid::createPyramid(){
    double cx = center.getX();
    double cy = center.getY();
    double cz = center.getZ();
    Vect base_center = Vect(cx, cy, cz); 
    Vect top = Vect(cx, cy+height, cz);
    
    corners.push_back(base_center);
    corners.push_back(top);
    double amnt = 6.28318 / sides;
    double offset = amnt / 2.0;
    for (int i = 0; i < sides; i++){
        Vect c = Vect(cx+ radius*cos(amnt*i + offset), cy, cz+ radius*sin(amnt*i + offset));
        corners.push_back(c);
    }
    for (int i = 0; i < corners.size(); i++){
        corners[i].print();
    }
     //base 
    for(int i = 2; i < corners.size(); i++){
        int i1 = i;
        int i2 = i+1;
        if(i2 == corners.size()){
            i2 = 2;
        }
        Vect* corner1 = &corners[i1];
        Vect* corner2 = &corners[i2];
        Triangle t1 = Triangle(&corners[0], corner2, corner1, color); // downward-facing base cap
        Triangle t2 = Triangle(&corners[1], corner1, corner2, color);

        triangleOs.push_back(t1);
        triangleOs.push_back(t2);
        
    }
    for(int i = 0; i< triangleOs.size(); i++){
        triangles.push_back(&triangleOs[i]);
    }

    
}
Color Pyramid::getColor(){return color;}
Color Pyramid::getColor(Vect p){return color;}
Vect Pyramid::getNormalAt(Vect point){
    // Use last_hit_index set by findIntersection — side triangles are at odd indices.
    // Compute the outward normal directly from the triangle's geometry.
    int i = last_hit_index;
    if (i >= 0 && i < (int)triangles.size()) {
        return triangles.at(i)->getTriangleNormal();
    }
    return triangles.at(1)->getTriangleNormal();
}
double Pyramid::findIntersection(Ray ray) {
    vector<double> intersections;
    for (int index = 0; index < triangles.size(); index++) {
        double t = triangles.at(index)->findIntersection(ray);
        intersections.push_back(t > 0.01 ? t : -1);
    }
    int index_of_winning_object = Raytracer::closestObjectIndex(intersections);
    if (index_of_winning_object >= 0) {
        last_hit_index = index_of_winning_object;
        return intersections.at(index_of_winning_object);
    }
    return -1;
}
void Pyramid::translate(Vect v){
    center = center.add(v);
    for (int index = 0; index < triangleOs.size(); index++) {
        triangleOs.at(index).translate(v);
    }
}
void Pyramid::rotate(Matrix r){
    for (int index = 0; index < triangles.size(); index++) {
        triangles.at(index)->rotate(r);
    }
    Matrix4x4 m(r);
    center = m.mult(center);
}




Pyramid::Pyramid(){
    center = Vect(0,0,0);
    color = Color(1,1,1,0);
    sides = 3;
    radius = 1;
    height = 1;
    createPyramid();
}

Pyramid::Pyramid(Vect position, double s , double r, double h, Color col){
    center = position;
    sides = s;
    radius = r;
    height = h;
    color = col;
    createPyramid();
}