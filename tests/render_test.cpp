#include "Raytracer.h"
#include "Plane.h"
#include "Light.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Rectangle.h"
#include "Pyramid.h"
#include "TriangleMesh.h"
#include "Texture.h"
#include "Quad.h"
#include <iostream>
#include <string>
using namespace std;

// Each test scene is identified by name and renders to a PNG.
// Usage: render_test <scene_name> <output_path>

static Light make_key_light() {
    return Light(Vect(-6, 10, -4), Color(0.8, 0.8, 0.8, 0));
}

// Sphere with no reflection, no floor
static void scene_sphere_plain(const string& outpath) {
    vector<Source*> lights;
    Light light = make_key_light();
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere(Vect(0, 0, 0), 1.0, Color(0.8, 0.2, 0.2, 0.0));
    objects.push_back(&sphere);

    Vect campos(0, 2, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Sphere with hard shadow on a floor
static void scene_sphere_shadow(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(-4, 8, -4), Color(0.9, 0.9, 0.9, 0));
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere(Vect(0, 0, 0), 1.0, Color(0.8, 0.2, 0.2, 0.0));
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
    Light light = make_key_light();
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
    Light light = make_key_light();
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

// Soft shadow — area light with radius
static void scene_sphere_soft_shadow(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(-4, 8, -4), Color(0.9, 0.9, 0.9, 0), 0.2);
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere(Vect(0, 0, 0), 1.0, Color(0.8, 0.2, 0.2, 0.0));
    Plane floor(Vect(0, 1, 0), -1.5, Color(0.7, 0.7, 0.7, 0.0));
    objects.push_back(&sphere);
    objects.push_back(&floor);

    Vect campos(0, 2, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Cone on a floor
static void scene_cone(const string& outpath) {
    vector<Source*> lights;
    Light light = make_key_light();
    lights.push_back(&light);

    vector<Object*> objects;
    // Cone axis is Z: apex at z=max, base at z=min.
    // rotateX(-pi/2) maps +Z -> +Y, so apex ends up at +Y (top), base at -Y (bottom).
    // position shifts the whole thing: base lands at y=-1.5 (on the floor).
    Cone cone(-1.5, 0.0, Color(0.3, 0.5, 0.8, 0.0));
    cone.rotateX(-M_PI / 2);
    cone.position = Vect(0, 1.5, 0);  // shift down so base is at y=-1.5
    Plane floor(Vect(0, 1, 0), -1.5, Color(0.7, 0.7, 0.7, 0.0));
    objects.push_back(&cone);
    objects.push_back(&floor);

    Vect campos(0, 2, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Cylinder on a floor
static void scene_cylinder(const string& outpath) {
    vector<Source*> lights;
    Light light = make_key_light();
    lights.push_back(&light);

    vector<Object*> objects;
    Cylinder cyl(-1.5, 1.5, 0.8, Color(0.2, 0.7, 0.4, 0.0));
    Plane floor(Vect(0, 1, 0), -1.5, Color(0.7, 0.7, 0.7, 0.0));
    objects.push_back(&cyl);
    objects.push_back(&floor);

    Vect campos(0, 2, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Rectangle (box) on a floor
static void scene_rectangle(const string& outpath) {
    vector<Source*> lights;
    Light light = make_key_light();
    lights.push_back(&light);

    vector<Object*> objects;
    Rectangle rect(Vect(0, 0, 0), 1.5, 1.5, 1.5, Color(0.7, 0.4, 0.2, 0.0));
    Plane floor(Vect(0, 1, 0), -1.5, Color(0.7, 0.7, 0.7, 0.0));
    objects.push_back(&rect);
    objects.push_back(&floor);

    Vect campos(2, 3, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Pyramid on a floor
static void scene_pyramid(const string& outpath) {
    vector<Source*> lights;
    Light light = make_key_light();
    lights.push_back(&light);

    vector<Object*> objects;
    Pyramid pyr(Vect(0, -1.5, 0), 4, 1.2, 2.5, Color(0.8, 0.7, 0.2, 0.0));
    Plane floor(Vect(0, 1, 0), -1.5, Color(0.7, 0.7, 0.7, 0.0));
    objects.push_back(&pyr);
    objects.push_back(&floor);

    Vect campos(0, 2, -5);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Checkerboard floor with a sphere
static void scene_checkerboard(const string& outpath) {
    vector<Source*> lights;
    Light light = make_key_light();
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere(Vect(0, 0, 0), 1.0, Color(0.8, 0.2, 0.2, 0.0));
    Plane floor(Vect(0, 1, 0), -1.5, Color(0, 0, 0, 0));
    CheckerboardTexture checker(Color(0.9, 0.9, 0.9, 0.0), Color(0.1, 0.1, 0.1, 0.0), 1.0);
    floor.texture = &checker;
    objects.push_back(&sphere);
    objects.push_back(&floor);

    Vect campos(0, 6, -4);
    Vect lookat(0, 0, 2);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}


static void noise_sphere_scene(const string& outpath, FastNoise::NoiseType type, float freq, int octaves = 3) {
    vector<Source*> lights;
    Light light = make_key_light();
    lights.push_back(&light);

    vector<Object*> objects;
    Sphere sphere(Vect(0, 0, 0), 1.5, Color(0, 0, 0, 0));
    // scale=10 maps UV [0,1] to noise space [0,10] so we see several noise features
    NoiseTexture noise(10.0, 1337,
        Color(0.1, 0.1, 0.1, 0.0),
        Color(0.9, 0.9, 0.9, 0.0));
    noise.setNoiseType(type);
    noise.setFrequency(freq);
    noise.setOctaves(octaves);
    sphere.Object::texture = &noise;
    objects.push_back(&sphere);

    Vect campos(0, 0, -4);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

static void scene_quad(const string& outpath) {
    vector<Source*> lights;
    Light light = make_key_light();
    lights.push_back(&light);

    vector<Object*> objects;
    Quad quad(Vect(0, 0, 0), 4.0, 4.0, Color(0.4, 0.6, 0.9, 0.0));
    objects.push_back(&quad);

    Vect campos(0, 5, -4);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

static void scene_image_texture(const string& outpath) {
    vector<Source*> lights;
    Light light(Vect(0, 20, 0), Color(1.0, 1.0, 1.0, 0));
    lights.push_back(&light);

    vector<Object*> objects;
    Quad quad(Vect(0, 0, 0), 10.0, 10.0, Color(0, 0, 0, 0));
    ImageTexture tex("images/test.png");
    quad.texture = &tex;
    objects.push_back(&quad);

    Vect campos(0, 12, -1);
    Vect lookat(0, 0, 0);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

// Showcase: reflective spheres on checkerboard, three colored lights with soft shadows
static void scene_showcase(const string& outpath) {
    vector<Source*> lights;
    // Warm key light — upper left
    Light key(Vect(-8, 12, -6), Color(1.0, 0.75, 0.45, 0), 0.5);
    // Cool fill light — upper right
    Light fill(Vect(10, 8, -2), Color(0.35, 0.55, 1.0, 0), 0.4);
    // Bright white rim — behind the spheres
    Light rim(Vect(0, 6, 10), Color(0.9, 0.95, 1.0, 0), 0.3);
    lights.push_back(&key);
    lights.push_back(&fill);
    lights.push_back(&rim);

    vector<Object*> objects;

    // Checkerboard floor
    Plane floor(Vect(0, 1, 0), -1.5, Color(0, 0, 0, 0));
    CheckerboardTexture checker(Color(0.92, 0.92, 0.92, 0.0), Color(0.08, 0.08, 0.08, 0.0), 1.5);
    floor.texture = &checker;
    objects.push_back(&floor);

    // Five reflective spheres: center + four around it
    Sphere s_center(Vect( 0.0,  0.0,  0.0), 1.0, Color(0.95, 0.95, 0.95, 0.8)); // near-mirror
    Sphere s_left  (Vect(-2.8,  0.0,  1.0), 0.9, Color(0.9,  0.25, 0.2,  0.6)); // red, reflective
    Sphere s_right (Vect( 2.8,  0.0,  1.0), 0.9, Color(0.2,  0.45, 0.9,  0.6)); // blue, reflective
    Sphere s_back_l(Vect(-1.6,  0.0,  2.8), 0.75, Color(0.25, 0.8,  0.35, 0.5)); // green
    Sphere s_back_r(Vect( 1.6,  0.0,  2.8), 0.75, Color(0.85, 0.75, 0.15, 0.5)); // gold
    objects.push_back(&s_center);
    objects.push_back(&s_left);
    objects.push_back(&s_right);
    objects.push_back(&s_back_l);
    objects.push_back(&s_back_r);

    Vect campos(0, 7, -9);
    Vect lookat(0, 0, 1);

    Raytracer tracer;
    tracer.generate(objects, lights, outpath, 1, campos, lookat, true);
}

static void scene_noise_simplex(const string& outpath)         { noise_sphere_scene(outpath, FastNoise::Simplex,        4.0f); }
static void scene_noise_simplex_fractal(const string& outpath) { noise_sphere_scene(outpath, FastNoise::SimplexFractal, 2.0f, 5); }
static void scene_noise_value(const string& outpath)           { noise_sphere_scene(outpath, FastNoise::Value,          4.0f); }
static void scene_noise_cellular(const string& outpath)        { noise_sphere_scene(outpath, FastNoise::Cellular,       4.0f); }

// Triangle mesh (teapot)
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
        cerr << "Scenes: sphere_plain, sphere_shadow, sphere_two, sphere_two_reflective,\n";
        cerr << "        sphere_soft_shadow, cone, cylinder, rectangle, pyramid, mesh\n";
        return 1;
    }
    string scene = argv[1];
    string outpath = argv[2];

    if      (scene == "sphere_plain")          scene_sphere_plain(outpath);
    else if (scene == "sphere_shadow")         scene_sphere_shadow(outpath);
    else if (scene == "sphere_two")            scene_sphere_two(outpath);
    else if (scene == "sphere_two_reflective") scene_sphere_two_reflective(outpath);
    else if (scene == "sphere_soft_shadow")    scene_sphere_soft_shadow(outpath);
    else if (scene == "cone")                  scene_cone(outpath);
    else if (scene == "cylinder")              scene_cylinder(outpath);
    else if (scene == "rectangle")             scene_rectangle(outpath);
    else if (scene == "pyramid")               scene_pyramid(outpath);
    else if (scene == "checkerboard")          scene_checkerboard(outpath);
    else if (scene == "quad")                     scene_quad(outpath);
    else if (scene == "image_texture")            scene_image_texture(outpath);
    else if (scene == "noise_simplex")            scene_noise_simplex(outpath);
    else if (scene == "noise_simplex_fractal")    scene_noise_simplex_fractal(outpath);
    else if (scene == "noise_value")              scene_noise_value(outpath);
    else if (scene == "noise_cellular")           scene_noise_cellular(outpath);
    else if (scene == "mesh")                  scene_mesh(outpath);
    else if (scene == "showcase")              scene_showcase(outpath);
    else {
        cerr << "Unknown scene: " << scene << "\n";
        return 1;
    }
    return 0;
}
