
#include "Raytracer.h"
#include <iostream>
#include "Plane.h"
#include "Pyramid.h"
#include "Cylinder.h"
#include "Light.h"
#include "Matrix4x4.h"
#include "Rectangle.h"
#include "Sphere.h"
#include "Cone.h"
#include "TriangleMesh.h"
#include "bmp.h"
#include "Magick++.h"
#include "FastNoise.h"
using namespace std;


void animateY(vector <Object*> scene_objects){
    Matrix m = Matrix(Vect(0,1,0), 3.1415926/25);
     
    //scene_objects[0] -> rotateY(3.1415926/50);
   // m = m.vectMult(campos);
    
    
}
void animateX(vector <Object*> scene_objects){
 
           scene_objects[0] -> rotateX(3.1415926/50);
   
    
    
}

int main (int argc, char *argv[]){
    Vect O (0,0,0);
     Vect campos (-3,6, -10);
    Color white_light (0.9, 0.9, 0.9, 0);
    Color green_light (.5, 1.0, .5, 0);
    Color gray_light (.5, .5, .5, 0);
    Color black_light (0, 0, 0, 0);
    Color maroon_light (0.5, 0.25, 0.25, 0);
    Color black ( 0.1,0.1,0.1,0.0);
    Color shiny_black ( 0.4,0.4,0.4,0.2);
    Color shiny_white ( .9,0.9,0.9,0.1);
    Color shiny_blue ( 0.3,0.3,0.7,0.9);
    Color shiny_gold ( 0.85,0.64,0.125,.1);
    Color mirror ( 0,0,0,.8);
    Color white ( 0.9,0.9,0.9,0);
    //Color yellow ( 1,1,0.5,0);
    Color shiny_green ( 0.2,0.9,0.2,0.3);
   // Color shiny_blue ( 0.2,0.2,0.6,0.1);
   // Color blue (0, 0, 1, 0);
    Color reflective (0,0,0,1);
    Color tiles (0,0,0,0,0,2);
    Color back_lines (0,0,0,0,0,3);
    Color fresnel (0,0,0,1,1,0);

    Vect light_pos (-6, 6, -4);
    
   
    
    vector<Source*> light_sources;
    vector <Object*> scene_objects;
    //scene lights 
    Light scene_light (light_pos, white_light);
    light_sources.push_back(dynamic_cast<Source*>(&scene_light));
    //scene objects 

    Plane scene_plane (Vect(0,1,0), 0, shiny_blue);

    scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));

    //TriangleMesh cup ("CUP.obj", white);
    //scene_objects.push_back(dynamic_cast<Object*>(&cup));

    
         // Create base image 
     Magick::Image image(Magick::Geometry(512,512), "white");
       // Set the image type to TrueColor DirectClass representation.
 

     // Set pixel at position 108,94 to red 
     //*(view.get(108,94,1,1)) = Magick::Color("red"); 

     // Save changes to image.
     //view.sync();


    TriangleMesh i ("teapot.obj", &image);
    Sphere j (Vect(0,2,0), 2, &image);
     scene_objects.push_back(dynamic_cast<Object*>(&j));
     //i.scale(0.5,0.5,0.5);

    // i.translate(Vect(0,0,0));
     Matrix m (Vect(0,1,0), 3.14159);
     //i.rotate(m);

   ///
    image.write( "x.gif" ); 



    /*
    Plane scene_plane (Vect(0,1,0), -3, maroon_light);
    scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));

    Plane scene_back (Vect(0,0,-1), -12, back_lines);
    scene_objects.push_back(dynamic_cast<Object*>(&scene_back));

    Cone cone = Cone(trans);
    scene_objects.push_back(dynamic_cast<Object*>(&cone));  
    Cylinder cyl = Cylinder(green_light);
    scene_objects.push_back(dynamic_cast<Object*>(&cyl)); 

    scene_objects.push_back(dynamic_cast<Object*>(&cone));
    cone.rotateX(3.14159/2);
    cone.scale(2,39,2);  
    cone.translate(Vect(0,-37,-0));
    cyl.scale(.2,.2,3);
    cyl.rotateY(-.3);
    cyl.rotateX(3.14159/2);
    cyl.translate(Vect(0,1,0));
    
      
    cyl.translate(Vect(0,0,-1));
    */
    /*
    Cylinder c1 = Cylinder(-3,3,0.05,shiny_white);
    scene_objects.push_back(dynamic_cast<Object*>(&c1));
    
    Cylinder c2 = Cylinder(-3,3,0.05,shiny_white);
    scene_objects.push_back(dynamic_cast<Object*>(&c2));

    Cylinder c3 = Cylinder(-3,3,0.05,shiny_white);
    scene_objects.push_back(dynamic_cast<Object*>(&c3));

    Rectangle r1 = Rectangle(Vect(0,0,0),1,1,1,black);
    scene_objects.push_back(dynamic_cast<Object*>(&r1));

    c2.rotateY(3.14159/2);
    c3.rotateX(3.14159/2);

    Sphere s1 = Sphere(Vect(-3, 0, 0), 1, shiny_black);
    Sphere s2 = Sphere(Vect(3, 0, 0), 1, shiny_black);
    Sphere s3 = Sphere(Vect(0, -3, 0), 1, shiny_black);
    Sphere s4 = Sphere(Vect(0, 3, 0), 1, shiny_black);
    Sphere s5 = Sphere(Vect(0, 0, -3), 1, shiny_black);
    Sphere s6 = Sphere(Vect(0, 0, 3), 1, shiny_black);
    scene_objects.push_back(dynamic_cast<Object*>(&s1));
    scene_objects.push_back(dynamic_cast<Object*>(&s2));
    scene_objects.push_back(dynamic_cast<Object*>(&s3));
    scene_objects.push_back(dynamic_cast<Object*>(&s4));
    scene_objects.push_back(dynamic_cast<Object*>(&s5));
    scene_objects.push_back(dynamic_cast<Object*>(&s6));
    */
    //c2.translate(Vect(2,1,0))
    
    //create scene
    Raytracer tracer = Raytracer();
    std::string name;
    double f = 0;
    for (double i = 0; i < 1; i++){
        name = "pictures/scene" + std::to_string((int)(i)) + ".bmp";
        cout << name << endl;
        cout << "CALLING GENERATE \n";
        
        //campos = m.vectMult(campos);
        image.type(Magick::TrueColorType);
     // Ensure that there is only one reference to underlying image 
         // If this is not done, then image pixels will not be modified.
         image.modifyImage();

         // Allocate pixel view 
         Magick::Pixels view(image);

         // Set all pixels in region anchored at 38x36, with size 160x230 to green. 
         size_t columns = 512; size_t rows = 512; 
         FastNoise fn;
         //fn.SetCellularNoiseLookup();
         //fn.SetCellularDistanceFunction(FastNoise::Natural);
         
         fn.SetNoiseType(FastNoise::Cellular);
         fn.SetFrequency(0.5);
         fn.SetCellularReturnType(FastNoise::Distance2Div);
         //fn.SetFractalOctaves(5);
         //fn.SetFractalLacunarity(float lacunarity) { m_lacunarity = lacunarity; }
         int t = i;
         t = i-200;
         f = f + t/4950.0;


         Magick::PixelPacket *pixels = view.get(0,0,columns,rows); 
         cout << f << endl;
        for ( int row = 0; row < 512; ++row ) {
            for ( int column = 0; column < 512 ; ++column ){
                Magick::Color c;
                
                double x = column; //* 3.14159  * 2;
             //   x = sin(x + 3.14159/50 * i);
                double y = row; //* 3.14159 *2 ;
              //  y = sin(y + 3.14159/50 * i);
                
                double r;
                double g;
                double b;
                fn.SetCellularReturnType(FastNoise::Distance2Div);
                 fn.SetFrequency(.2);
                float n  = fn.GetNoise(x,y); 
                n = n * n;
                //n = n+1; 
                            
                if(n  < .2){r = 0; g= 0; b = 0;}
                else{
                    fn.SetCellularReturnType(FastNoise::CellValue);
                    fn.SetFrequency(0.04);
                    n =  fn.GetNoise(x,y);
                   
                    if( n < -.9) {r  = 1; g  = 3; b = 3; }
                    if( n >= -.9 && n < -.8) {r  = .7; g  = .2; b = .2; }
                    if( n >= -.8 && n < -.7) {r  = .5; g  = .3; b = .3; }
                    if( n >= -.7 && n < -.6) {r  = .2; g  = .5; b = .5; }
                    if( n >= -.6 && n < -.5) {r  = .1; g  = .7; b = .7; }
                    if( n >= -.5 && n < -.4) {r  = 0; g  = .9; b = .9; }
                    if( n >= -.4 && n < -.3) {r  = 0; g  = .9; b = .5; }
                    if( n >= -.3 && n < -.2) {r  = 0; g  = .9; b = .3; }
                    if( n >= -.2 && n < -.1) {r  = .5; g  = .9; b = .5; }
                    if( n >= -.1 && n < -.0)  {r  = 0; g  = .5; b = .9; }

                    if( n > 9) {r  = .1; g  = .3; b = .3; }
                    if( n >= .8&& n < .9) {r  = .2; g  = .2; b = .2; }
                    if( n >= .7 && n < .8) {r  = .5; g  = .5; b = .3; }
                    if( n >= .6 && n < .7) {r  = .2; g  = .1; b = .5; }
                    if( n >= .5 && n < .6) {r  = .1; g  = .2; b = .4; }
                    if( n >= .4 && n < .5) {r  = .1; g  = .9; b = .8; }
                    if( n >= .3 && n < .4) {r  = .4; g  = .5; b = .6; }
                    if( n >= .2 && n < .3) {r  = .3; g  = .8; b = .2; }
                    if( n >= .1 && n < .2) {r  = .8; g  = .4; b = .4; }
                    if( n >= 0 && n < .1)  {r  = .2; g  = .7; b = .1; }
                    //r = n; g = n; b = n;
                }
                //r = n; g = n; b = n;      

                c =  Magick::ColorRGB(r,g,b); 
                
                
                *(view.get(column,row,1,1)) = c;
                view.sync();
             }
      
         }
           image.write( "x.gif" ); 
           cout << "writing image" << endl;
           bool threaded = true;

        tracer.generate(scene_objects, light_sources, name, 1, campos, O, threaded);
        
       
    }
    for (double i = 100; i < 0; i++){
        name = "pictures/scene" + std::to_string((int)(i)) + ".bmp";
        cout << name << endl;
        cout << "CALLING GENERATE \n";
        animateX(scene_objects);
        //tracer.generate(scene_objects, light_sources, name, 1);
       
    }
}

 /* 
                    if( n < -.9) {r  = 1; g  = 3; b = 3; }
                    if( n >= -.9 && n < -.8) {r  = .7; g  = .2; b = .2; }
                    if( n >= -.8 && n < -.7) {r  = .5; g  = .3; b = .3; }
                    if( n >= -.7 && n < -.6) {r  = .2; g  = .5; b = .5; }
                    if( n >= -.6 && n < -.5) {r  = .1; g  = .7; b = .7; }
                    if( n >= -.5 && n < -.4) {r  = 0; g  = .9; b = .9; }
                    if( n >= -.4 && n < -.3) {r  = 0; g  = .9; b = .5; }
                    if( n >= -.3 && n < -.2) {r  = 0; g  = .9; b = .3; }
                    if( n >= -.2 && n < -.1) {r  = .5; g  = .9; b = .5; }
                    if( n >= -.1 && n < -.0)  {r  = 0; g  = .5; b = .9; }

                    if( n > 9) {r  = .1; g  = .3; b = .3; }
                    if( n >= .8&& n < .9) {r  = .2; g  = .2; b = .2; }
                    if( n >= .7 && n < .8) {r  = .5; g  = .5; b = .3; }
                    if( n >= .6 && n < .7) {r  = .2; g  = .1; b = .5; }
                    if( n >= .5 && n < .6) {r  = .1; g  = .2; b = .4; }
                    if( n >= .4 && n < .5) {r  = .1; g  = .9; b = .8; }
                    if( n >= .3 && n < .4) {r  = .4; g  = .5; b = .6; }
                    if( n >= .2 && n < .3) {r  = .3; g  = .8; b = .2; }
                    if( n >= .1 && n < .2) {r  = .8; g  = .4; b = .4; }
                    if( n >= 0 && n < .1)  {r  = .2; g  = .7; b = .1; }
                    */
