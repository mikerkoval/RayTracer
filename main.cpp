
#include "Raytracer.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include "Plane.h"
#include "Light.h"
#include "Rectangle.h"
#include "Pyramid.h"
#include "Cone.h"
#include "Cylinder.h"
#include "TriangleMesh.h"
#include "Matrix.h"

using namespace std;

int main(int argc, char *argv[]) {

    mkdir("output", 0755);

    // --- Lights ---
    vector<Source*> light_sources;
    Light key_light  (Vect(-6, 10, -4), Color(0.6, 0.6, 0.6, 0));
    light_sources.push_back(dynamic_cast<Source*>(&key_light));

    // --- Static scene ---
    vector<Object*> scene_objects;

    Plane floor     (Vect( 0,  1,  0),   0,  Color(0.80, 0.75, 0.60, 0.05));
    Plane ceiling   (Vect( 0, -1,  0),  -16, Color(0.02, 0.02, 0.02, 0.0));
    Plane back_wall (Vect( 0,  0, -1),  -20, Color(0.30, 0.35, 0.65, 0.0));
    Plane left_wall (Vect( 1,  0,  0),  -12, Color(0.65, 0.15, 0.15, 0.0));
    Plane right_wall(Vect(-1,  0,  0),  -12, Color(0.15, 0.55, 0.25, 0.0));

    scene_objects.push_back(dynamic_cast<Object*>(&floor));
    scene_objects.push_back(dynamic_cast<Object*>(&ceiling));
    scene_objects.push_back(dynamic_cast<Object*>(&back_wall));
    scene_objects.push_back(dynamic_cast<Object*>(&left_wall));
    scene_objects.push_back(dynamic_cast<Object*>(&right_wall));

    // Camera lower and more forward to catch blue back-wall reflections when cone is inverted
    Vect campos  (1.5, 10.0, -6);
    Vect look_at (0,    4.0,  5);

    // 360° Y rotation (spin around vertical axis), 90 frames
    int total_frames = 90;

    cout << "Total frames: " << total_frames << endl;

    for (int frame = 0; frame < total_frames; frame++) {
        double ry = 2.0 * M_PI * frame / total_frames;

        // Teapot: native coords centered ~(0,1.5,0), scale ~3 units tall.
        // rotateY spins it. position = world center elevated off floor.
        TriangleMesh teapot("obj/teapot_normals.obj", Color(0.95, 0.90, 0.85, 0.2));
        teapot.rotateY(ry);
        teapot.position = Vect(0, 3.0, 5);

        vector<Object*> frame_objects = scene_objects;
        frame_objects.push_back(dynamic_cast<Object*>(&teapot));

        ostringstream ss;
        ss << "output/frame" << setw(4) << setfill('0') << frame << ".png";
        string filename = ss.str();

        cout << "Rendering frame " << frame << "/" << total_frames << endl;
        Raytracer tracer;
        tracer.generate(frame_objects, light_sources, filename, 1, campos, look_at, true);
    }

    cout << "Done." << endl;
    return 0;
}
