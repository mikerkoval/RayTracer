#include "TriangleMesh.h"
#include "Raytracer.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
bool TriangleMesh::pointInTriangle(Triangle t, Vect p){
    Vect normal = t.getTriangleNormal();
    Vect A = t.getA();
    Vect B = t.getC();
    Vect C = t.getB();
    Vect Q = p;
    Vect QA (Q.getX()- A.getX(),Q.getY()- A.getY(), Q.getZ()- A.getZ()); 
    double test4 = QA.dotProduct(t.getTriangleNormal());


    Vect ba = B.add(A.negative()).negative();
    Vect ca = C.add(A.negative()).negative();

    Vect ap = A.add(p.negative()).negative();
    Vect bp = B.add(p.negative()).negative();
    Vect cp = C.add(p.negative()).negative();


    double areaABC = normal.dotProduct(ba.crossProduct(ca));
    double areaPBC = normal.dotProduct(bp.crossProduct(cp));
    double areaPCA = normal.dotProduct(cp.crossProduct(ap));


    if(areaABC < 0){areaABC = -areaABC;}
    if(areaPBC < 0){areaPBC = -areaPBC;}
    if(areaPCA < 0){areaPCA = -areaPCA;}
    

   
    //real areaABC = DOT( normal, CROSS( (b - a), (c - a) )  ) ;
    //real areaPBC = DOT( normal, CROSS( (b - P), (c - P) )  ) ;
    //real areaPCA = DOT( normal, CROSS( (c - P), (a - P) )  ) ;

    double u = areaPBC / areaABC ; // alpha
    double v = areaPCA / areaABC ; // beta
    double w = 1.0f - u - v ; // gamma
    Vect bary (u,v,w);
    bary = bary.normalize();
  
    u = bary.getX();
    v = bary.getY();
    w = bary.getZ();
    double a = .0000000000001;
    if(u >= 0 -a  && u <= 1 + a && v >= 0 - a && v <= 1 + a && w >= 0 - a && w <= 1 + a && test4 > -a  && test4 < a){
        return true;
    }
    else{
        return false;
    }
    /*
    Vect normal = t.getTriangleNormal();
    Vect A = t.getA();
    Vect B = t.getC();
    Vect C = t.getB();

    Vect n = normal;

    Vect ba = B.add(A.negative()).negative();
    Vect ca = C.add(A.negative()).negative();

    Vect ap = A.add(point.negative()).negative();
    Vect bp = B.add(point.negative()).negative();
    Vect cp = C.add(point.negative()).negative();


    double areaABC = n.dotProduct(ba.crossProduct(ca));
    double areaPBC = n.dotProduct(bp.crossProduct(cp));
    double areaPCA = n.dotProduct(cp.crossProduct(ap));


    if(areaABC < 0){areaABC = -areaABC;}
    if(areaPBC < 0){areaPBC = -areaPBC;}
    if(areaPCA < 0){areaPCA = -areaPCA;}
    

   
    //real areaABC = DOT( normal, CROSS( (b - a), (c - a) )  ) ;
    //real areaPBC = DOT( normal, CROSS( (b - P), (c - P) )  ) ;
    //real areaPCA = DOT( normal, CROSS( (c - P), (a - P) )  ) ;

    double u = areaPBC / areaABC ; // alpha
    double v = areaPCA / areaABC ; // beta
    double w = 1.0f - u - v ; // gamma


    if(u >= 0 && u <= 1 && v >= 0 && v <= 1 && w >= 0 && w <= 1){
        return true;
    }
    else{
        return false;
    }
    */
    

   
}
void TriangleMesh::createMesh(string c){
    cout << "creating mesh"  << endl;
    string line;
    ifstream myfile (c);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            vector<string> arr = split(line, ' ');
            if(arr[0] == "v"){
                Vect v (stod(arr[1]), stod(arr[2]), stod(arr[3]));
                corners.push_back(v);
            }
            if(arr[0] == "vt"){
                Vect v (stod(arr[1]), stod(arr[2]),0);
                textures.push_back(v);
            }
            if(arr[0] == "vn"){
                Vect v (stod(arr[1]), stod(arr[2]), stod(arr[3]));
                normals.push_back(v);
            }
            if(arr[0] == "f"){
                string a = split(arr[1], '/')[0];
                string b = split(arr[2], '/')[0];
                string c = split(arr[3], '/')[0];
                string at = split(arr[1], '/')[1];
                string bt = split(arr[2], '/')[1];
                string ct = split(arr[3], '/')[1];
                string an = split(arr[1], '/')[2];
                string bn = split(arr[2], '/')[2];
                string cn = split(arr[3], '/')[2];
                int c1 = stoi(a) -1; 
                int c2 = stoi(b) -1; 
                int c3 = stoi(c) - 1; 
               
                int n1 = stoi(an) -1; 
                int n2 = stoi(bn) -1; 
                int n3 = stoi(cn) -1; 
                Triangle t;
                if(setText){
                     int t1 = stoi(at) -1; 
                    int t2 = stoi(bt) -1; 
                    int t3 = stoi(ct) -1; 
                     t = Triangle(&corners[c1], &corners[c2], &corners[c3], normals[n1], normals[n2], normals[n3], textures[t1],textures[t2],textures[t3], texture);

                }
                else{
                     t = Triangle(&corners[c1], &corners[c2], &corners[c3], normals[n1], normals[n2], normals[n3], color);
                }

                triangleOs.push_back(t);

            }

        }
        myfile.close();
    }

    else cout << "Unable to open file \n"; 

    for(int i = 0; i< triangleOs.size(); i++){
        triangles.push_back(&triangleOs[i]);
    }
    
    double max = 0;
    for(int i = 0; i < corners.size(); i++){
        if(corners[i].magnitude() > max){
            max = corners[i].magnitude();
        }
    }
    boundingSphere = Sphere(Vect(), max , Color());





}
Color TriangleMesh::getColor(Vect p){

    for (int index = 0; index < triangles.size(); index++) {
        Triangle* op  =  triangles.at(index);
        Triangle o = *op;
        if(pointInTriangle(o, p)){
            return o.getColor(p);
        }

    }
    return Color(1,1,1,0);
}
Vect TriangleMesh::getNormalAt(Vect point){
    //cout << "getting normal " << endl;
    for (int index = 0; index < triangles.size(); index++) {
        Triangle* op  =  triangles.at(index);
        Triangle o = *op;
        if(pointInTriangle(o, point)){
            return o.getNormalAt(point);
        }

    }
    cout << "Missed "<< endl;
    return Vect(0,0,0);
}
double TriangleMesh::findIntersection(Ray ray) {
    if(boundingSphere.findIntersection(ray) == -1){return -1;}

    vector<double> intersections;      
    
    for (int index = 0; index < triangles.size(); index++) {
        Triangle* op  =  triangles.at(index);
        Triangle o = *op;
        Vect a = o.getC();

        intersections.push_back(o.findIntersection(ray));   
    }
    int index_of_winning_object = Raytracer::closestObjectIndex(intersections);
    if(index_of_winning_object >= 0){
        return intersections.at(index_of_winning_object);

    } 
    return -1;
}
void TriangleMesh::rotate(Matrix r){
    for (int index = 0; index < triangles.size(); index++) {
        Vect A = triangles.at(index)->getA();
        Vect B = triangleOs.at(index).getA();

        triangles.at(index)->rotate(r);
    
    }
}

void TriangleMesh::translate(Vect p){
    for (int index = 0; index < triangles.size(); index++) {
   

        triangles.at(index)->translate(p);
    
    }
    boundingSphere.translate(p);
}
void TriangleMesh::scale(double x, double y, double z){
    for (int index = 0; index < triangles.size(); index++) {
        

        triangles.at(index)->scale(x,y,z);
    
    }
    double max = x; 
    if(y > max){max = y;}
    if(z < max){max = z;}
    boundingSphere.scale(max);

}
bool TriangleMesh::getCL(){
    return clearLight;

}

TriangleMesh::TriangleMesh(){
    center = Vect(0,0,0);
    color = Color(1,1,1,0);
    setText = false;
    clearLight = false;
    createMesh("sphere.obj");
}


TriangleMesh::TriangleMesh(string file, Color c){
    center = Vect(0,0,0);
    color = c;
    setText = false;
    clearLight = false;
    createMesh(file);
}
TriangleMesh::TriangleMesh(string file, Color c, bool l){
    center = Vect(0,0,0);
    color = c;
    setText = false;
    clearLight = l;
    createMesh(file);
}
TriangleMesh::TriangleMesh(string file, Magick::Image* c){
    center = Vect(0,0,0);
    texture = c;
    setText = true;
    clearLight = false;
    createMesh(file);

}
TriangleMesh::TriangleMesh(string file, Magick::Image* c, bool l){
    center = Vect(0,0,0);
    texture = c;
    setText = true;
    clearLight = l;
    createMesh(file);
    
}
