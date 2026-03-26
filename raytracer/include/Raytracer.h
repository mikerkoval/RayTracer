#ifndef _RAYTRACER_H
#define _RAYTRACER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <unistd.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"

#include "Object.h"

#include "Source.h"

#include <thread>
using namespace std;
  
struct RGBType {
    double r;
    double g;
    double b;
};
class Raytracer {

    clock_t t1, t2;
  

    int dpi;
    int width;
    int height;
    int n;
    RGBType *pixels;

    int aadepth;
    double aathreshold;
    double aspectratio;
    double ambientlight;
    double accuracy;
    Vect O ;
    Vect X ;
    Vect Y ;
    Vect Z ;

    Vect campos ;
    Vect look_at ;
    Vect diff_btw ;
    
    Vect camdir;
    Vect camright;
    Vect camdown ;
    Camera scene_cam ;




    ////////constructors 
    public:
    Raytracer();
    // methods
    static int closestObjectIndex(vector<double> intersections);

    static Color getColorAt(Vect intersection_position,Vect intersecting_direction, vector<Object*> scene_objects, int index_closest,vector<Source*> light_sources,double  accuracy,double ambientlight, int n);

    
    int generate (vector<Object*> objs, vector<Source*>lights, std::string filename, int aa, Vect, Vect, bool);

    void savebmp (const char *filename, int w, int h, int dpi, RGBType *data, int size);


};


#endif