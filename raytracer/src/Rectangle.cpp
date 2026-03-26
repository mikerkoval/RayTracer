#include "Rectangle.h"
#include <iostream>
using namespace std;
bool Rectangle::pointInTriangle(Triangle t, Vect p){
    Vect normal = t.getNormalAt(p);
    Vect A = t.getA();
    Vect B = t.getC();
    Vect C = t.getB();

    Vect Q = p;
    Vect QA (Q.getX()- A.getX(),Q.getY()- A.getY(), Q.getZ()- A.getZ()); 
  

    Vect QC (Q.getX()- C.getX(),Q.getY()- C.getY(), Q.getZ()- C.getZ()); 
 
    Vect QB (Q.getX()- B.getX(),Q.getY()- B.getY(), Q.getZ()- B.getZ()); 
    QA = QA.negative();
    QB = QB.negative();
    QC = QC.negative();

    double a1 = acos( QA.dotProduct(QB) / (QA.magnitude()* QB.magnitude()));
    double a2 = acos( QB.dotProduct(QC) / (QB.magnitude()* QC.magnitude()));
    double a3 = acos( QC.dotProduct(QA) / (QC.magnitude()* QA.magnitude()));
    if(a1 < 0){ a1 = -1;}
    if(a2 < 0){ a2 = -1;}
    if(a3 < 0){ a3 = -1;}

    double test4 = QA.dotProduct(t.getNormalAt(p));
    if(a1 + a2 + a3 < 3.1415926 * 2 + .0001 &&  a1 + a2 + a3 < 3.1415926 * 2 - .0001 && test4 < .00001 && test4 > -.00001){
        return true;
    }
    else{
        return false;
    }

    
    

   
}
void Rectangle::createRectangle(){
    double cx = center.getX();
    double cy = center.getY();
    double cz = center.getZ();
    double w = width/2;
    double h = height/2;
    double l = length/2;
     c1 = Vect(cx-w, cy+h, cz+l);
     c2 = Vect(cx+w, cy+h, cz+l);
     c3 = Vect(cx-w, cy-h, cz+l);
     c4 = Vect(cx+w, cy-h, cz+l);

   

     c5 = Vect(cx-w, cy+h, cz-l);
     c6 = Vect(cx+w, cy+h, cz-l);
     c7 = Vect(cx-w, cy-h, cz-l);
     c8 = Vect(cx+w, cy-h, cz-l);

    corners.push_back(c1); corners.push_back(c2);
    corners.push_back(c3); corners.push_back(c4);
    corners.push_back(c5); corners.push_back(c6);
    corners.push_back(c7); corners.push_back(c8);

 
    
     //front 
     t1 = Triangle(&corners[0], &corners[1], &corners[2], color);
     t2 = Triangle(&corners[1], &corners[3], &corners[2], color);
    //back
     t3 = Triangle(&corners[4], &corners[6], &corners[5], color);
     t4 = Triangle(&corners[5], &corners[6], &corners[7], color);
    //left
     t5 = Triangle(&corners[0], &corners[2], &corners[4], color);
     t6 = Triangle(&corners[4], &corners[2], &corners[6], color);
    //right
     t7 = Triangle(&corners[5], &corners[7], &corners[1], color);
     t8 = Triangle(&corners[1], &corners[7], &corners[3], color);
    //top
     t9 = Triangle(&corners[0], &corners[4], &corners[1], color);
     t10 = Triangle(&corners[4], &corners[5], &corners[1], color);
    //bottom
     t11 = Triangle(&corners[2], &corners[3], &corners[6], color);
     t12 = Triangle(&corners[3], &corners[7], &corners[6], color);


    triangles.push_back(dynamic_cast<Triangle*>(&t1));
    triangles.push_back(dynamic_cast<Triangle*>(&t2));
    triangles.push_back(dynamic_cast<Triangle*>(&t3));
    triangles.push_back(dynamic_cast<Triangle*>(&t4));
    triangles.push_back(dynamic_cast<Triangle*>(&t5));
    triangles.push_back(dynamic_cast<Triangle*>(&t6));
    triangles.push_back(dynamic_cast<Triangle*>(&t7));
    triangles.push_back(dynamic_cast<Triangle*>(&t8));
    triangles.push_back(dynamic_cast<Triangle*>(&t9));
    triangles.push_back(dynamic_cast<Triangle*>(&t10));
    triangles.push_back(dynamic_cast<Triangle*>(&t11));
    triangles.push_back(dynamic_cast<Triangle*>(&t12));
    
    
}
Color Rectangle::getColor(){return color;}

int Rectangle::move(Vect m){
    center = m;
    return 1;

}
Vect Rectangle::getNormalAt(Vect point){
    for (int index = 0; index < triangles.size(); index++) {
        Triangle* op  =  triangles.at(index);
        Triangle o = *op;
        if(pointInTriangle(o, point)){
            return o.getNormalAt(point);
        }
    }
    return Vect(0,0,0);
}
double Rectangle::findIntersection(Ray ray) {
    vector<double> intersections;      
    
    for (int index = 0; index < triangles.size(); index++) {
        Triangle* op  =  triangles.at(index);
        Triangle o = *op;
        intersections.push_back(o.findIntersection(ray));   
    }

    int index_of_winning_object = Raytracer::closestObjectIndex(intersections);

    if(index_of_winning_object >= 0){
        //cout << index_of_winning_object << endl;
        return intersections.at(index_of_winning_object);
    } 
    return -1;
}
void Rectangle::rotate(Matrix r){
    for (int index = 0; index < triangles.size(); index++) {
        triangles.at(index)->rotate(r);
    
    }
}
void Rectangle::rotateY(double s){
    Vect r (0,1,0);
    Matrix m (r, s);
    rotate(m);
}
void Rectangle::rotateX(double s){
    Vect r (1,0,0);
    Matrix m (r, s);
    rotate(m);
}


Rectangle::Rectangle(){
    center = Vect(0,0,0);
    color = Color(1,1,1,0);
    length = 1;
    width = 1;
    height = 1;
    createRectangle();
}

Rectangle::Rectangle(Vect position, double l , double w, double h, Color col){
    center = position;
    length = l;
    width = w;
    height = h;
    color = col;
    createRectangle();
}
