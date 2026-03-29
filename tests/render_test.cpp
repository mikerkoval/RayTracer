#include "Raytracer.h"
#include "Plane.h"
#include "Light.h"
#include "Sphere.h"
#include "TriangleMesh.h"
#include <iostream>
#include <string>
using namespace std;

// Each test scene is identified by name and renders to a PNG.
// Usage: render_test <scene_name> <output_path>

// Sphere with no reflection, no floor
static void scene_sphere_plain(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(-6, 10, -4), Color(0.8, 0.8, 0.8, 0));
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere(Vect(0, 0, 0), 1.0, Color(0.8, 0.2, 0.2, 0.0));
    objects.push_back(&sphere);

    Vect campos(0, 2, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Sphere with reflection, no floor
static void scene_sphere_reflective(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(-6, 10, -4), Color(0.8, 0.8, 0.8, 0));
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere(Vect(0, 0, 0), 1.0, Color(0.8, 0.2, 0.2, 0.5));
    objects.push_back(&sphere);

    Vect campos(0, 2, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Sphere with reflection on a reflective floor
static void scene_sphere_floor(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(-6, 10, -4), Color(0.8, 0.8, 0.8, 0));
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere(Vect(0, 0, 0), 1.0, Color(0.8, 0.2, 0.2, 0.3));
    Plane floor(Vect(0, 1, 0), -1.5, Color(0.7, 0.7, 0.7, 0.0));
    objects.push_back(&sphere);
    objects.push_back(&floor);

    Vect campos(0, 2, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Two spheres with shadows
static void scene_sphere_two(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(-6, 10, -4), Color(0.8, 0.8, 0.8, 0));
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere1(Vect(-1.2, 0, 0), 1.0, Color(0.2, 0.4, 0.8, 0.0));
    Sphere sphere2(Vect( 1.2, 0, 0), 1.0, Color(0.2, 0.8, 0.3, 0.0));
    Plane floor(Vect(0, 1, 0), -1.5, Color(0.7, 0.7, 0.7, 0.0));
    objects.push_back(&sphere1);
    objects.push_back(&sphere2);
    objects.push_back(&floor);

    Vect campos(0, 2, -6);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Two reflective spheres next to each other
static void scene_sphere_two_reflective(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(-6, 10, -4), Color(0.8, 0.8, 0.8, 0));
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere1(Vect(-1.2, 0, 0), 1.0, Color(0.2, 0.4, 0.8, 0.5));
    Sphere sphere2(Vect( 1.2, 0, 0), 1.0, Color(0.2, 0.8, 0.3, 0.5));
    objects.push_back(&sphere1);
    objects.push_back(&sphere2);

    Vect campos(0, 2, -6);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Glass sphere showcasing Fresnel reflection/refraction
static void scene_glass_sphere(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(-6, 10, -4), Color(0.9, 0.9, 0.9, 0));
    lights.push_back(&light);

    vector<Object*> objects;

    // Glass sphere
    Sphere glass(Vect(0, 0, 0), 1.5, Color(0.95, 0.95, 0.95, 1.0, 1.0, 0));

    // Large red back wall as a plane to guarantee refracted rays hit something red
    Plane back_wall(Vect(0, 0, -1), -8, Color(0.8, 0.15, 0.15, 0.0));
    Sphere back(Vect(0, 0, 4), 1.0, Color(0.8, 0.15, 0.15, 0.0));

    // Colored walls to make reflection clearly visible on the edges
    Plane floor(Vect(0, 1, 0), -1.5, Color(0.85, 0.85, 0.85, 0.0));
    Plane left_wall(Vect(1, 0, 0), -5, Color(0.7, 0.15, 0.15, 0.0));
    Plane right_wall(Vect(-1, 0, 0), -5, Color(0.15, 0.6, 0.2, 0.0));

    objects.push_back(&glass);
    objects.push_back(&back_wall);
    objects.push_back(&back);
    objects.push_back(&floor);
    objects.push_back(&left_wall);
    objects.push_back(&right_wall);

    Vect campos(0, 0.5, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

static void scene_mesh(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(-6, 10, -4), Color(0.7, 0.7, 0.7, 0));
    lights.push_back(&light);

    vector<Object*> objects;
    TriangleMesh teapot("obj/teapot_smooth.obj", Color(0.95, 0.90, 0.85, 0.0));
    teapot.position = Vect(0, 3.0, 5);
    Plane floor(Vect(0, 1, 0), 0, Color(0.80, 0.75, 0.60, 0.05));
    objects.push_back(&teapot);
    objects.push_back(&floor);

    Vect campos(1.5, 10.0, -6);
    Vect lookat(0, 4.0, 5);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: render_test <scene> <output.png>\n";
        cerr << "Scenes: sphere_plain, sphere_reflective, sphere_floor, sphere_two, mesh\n";
        return 1;
    }
    string scene = argv[1];
    string outpath = argv[2];

    if      (scene == "sphere_plain")      scene_sphere_plain(outpath);
    else if (scene == "sphere_reflective") scene_sphere_reflective(outpath);
    else if (scene == "sphere_floor")      scene_sphere_floor(outpath);
    else if (scene == "sphere_two")        scene_sphere_two(outpath);
    else if (scene == "sphere_two_reflective") scene_sphere_two_reflective(outpath);
    else if (scene == "glass_sphere")          scene_glass_sphere(outpath);
    else if (scene == "mesh")              scene_mesh(outpath);
    else {
        cerr << "Unknown scene: " << scene << "\n";
        return 1;
    }
    return 0;
}
