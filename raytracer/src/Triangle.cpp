#include "Triangle.h"
#include "Matrix4x4.h"
Vect Triangle::getA(){return A;}
Vect Triangle::getB(){return B;}
Vect Triangle::getC(){return C;}
Color Triangle::getColor(Vect point){
    if(setText){
        Vect n = getTriangleNormal();

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
        
        

        Vect uv =  (textA.mult(u)).add(textB.mult(v)).add(textC.mult(w));
        int width = texture ->columns();
        int height = texture ->rows();
 

        double x = width * (uv.getX()) ; x = (int) x;
        double y = height * (1-uv.getY()) ; y = (int) y;

        //vector<unsigned int> c = texture -> getPixel(x,y); 
        
        //return Color(c[0]/255.0,c[1]/255.0,c[2]/255.0,0);

        int row = y;
        int column = x;
        
        Magick::PixelPacket *pixels = texture->getPixels(0, 0, width, height);
        Magick::Color color = pixels[width * row + column];

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
Vect Triangle::getNormalAt(Vect point){
    if(setNorm){
        
        Vect n = getTriangleNormal();

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

        

        return (normalA.mult(u)).add(normalB.mult(v)).add(normalC.mult(w)).normalize();
    }
    return getTriangleNormal();
}
Vect Triangle::getTriangleNormal(){
    Vect CA (C.getX()- A.getX(),C.getY()- A.getY(), C.getZ()- A.getZ()); 
    Vect BA (B.getX()- A.getX(),B.getY()- A.getY(), B.getZ()- A.getZ()); 
    Vect normal = CA.crossProduct(BA).normalize();
    return normal;
}
double Triangle::getTriangleDistance(){
    normal = getTriangleNormal();
    distance = normal.dotProduct(A);
    return distance;
}
void Triangle::rotate(Matrix r){
    Matrix4x4 m(r);
    A = m.mult(A);
    B = m.mult(B);
    C = m.mult(C);
    m = m.inverse();
    m = m.transpose();
    normalA = m.mult(normalA);
    normalB = m.mult(normalB);
    normalC = m.mult(normalC);
}
void Triangle::translate(Vect v){
    A = A.add(v);

    B = B.add(v);
    C = C.add(v);

}

void Triangle::scale(double x, double y, double z){
    A.setX(A.getX() * x); 
    B.setX(B.getX() * x); 
    C.setX(C.getX() * x); 

    A.setY(A.getY() * y); 
    B.setY(B.getY() * y); 
    C.setY(C.getY() * y); 

    A.setZ(A.getZ() * z); 
    B.setZ(B.getZ() * z); 
    C.setZ(C.getZ() * z); 


}
double Triangle::findIntersection(Ray ray) {

    Vect ray_direction = ray.getDirection();
    Vect ray_origin = ray.getOrigin();
    normal = getTriangleNormal();

    distance = getTriangleDistance();
    double a = ray_direction.dotProduct(normal);
    
    if (a == 0) {
        // ray is parallel to the plane
        return -1;
    }
    else {
        // Möller–Trumbore intersection
        Vect v0v1 = B.add(A.negative());
        Vect v0v2 = C.add(A.negative());
        Vect pvec = ray.getDirection().crossProduct(v0v2);
        double det = v0v1.dotProduct(pvec);

        if (fabs(det) < 1e-8) return -1;

        double invDet = 1.0 / det;

        Vect tvec = ray.getOrigin().add(A.negative());
        double u = tvec.dotProduct(pvec) * invDet;
        if (u < -1e-6 || u > 1 + 1e-6) return -1;

        Vect qvec = tvec.crossProduct(v0v1);
        double v = ray.getDirection().dotProduct(qvec) * invDet;
        if (v < -1e-6 || u + v > 1 + 1e-6) return -1;

        double t = v0v2.dotProduct(qvec) * invDet;
        return t;
    }
}


Triangle::Triangle(){
    A = Vect(1,0,0);
    B = Vect(0,1,0);
    C = Vect(0,0,1);
    Ap = &A;
    Bp = &B;
    Cp = &C;
    normalA = Vect(1,0,0);
    distance  = 1;
    color = Color(.5,.5,.5,0);
   
}

Triangle::Triangle(Vect* a, Vect* b, Vect* c, Color col){
    Ap = a;
    Bp = b; 
    Cp = c;
    A = *Ap;
    B = *Bp;
    C = *Cp;
    color = col;
}
Triangle::Triangle(Vect* a, Vect* b, Vect* c, Vect na, Vect nb, Vect nc, Color col){
    Ap = a;
    Bp = b; 
    Cp = c;
    A = *Ap;
    B = *Bp;
    C = *Cp;
    normalA = na;
    normalB = nb;
    normalC = nc;
    setNorm = true;
    setText = false;
    color = col;
}
Triangle::Triangle(Vect* a, Vect* b, Vect* c, Vect na, Vect nb, Vect nc,Vect ta, Vect tb, Vect tc, Magick::Image* text){
    Ap = a;
    Bp = b; 
    Cp = c;
    A = *Ap;
    B = *Bp;
    C = *Cp;
    normalA = na;
    normalB = nb;
    normalC = nc;
    textA = ta;
    textB = tb;
    textC = tc;
    setNorm = true;
    setText = true;
    texture = text;

}
Triangle::Triangle(Vect a, Vect b, Vect c, Color col){
    A = a;
    B = b;
    C = c;

    Ap = &A;
    Bp = &B; 
    Cp = &C;
    setNorm = false;
    setText = false;
    color = col;
}
