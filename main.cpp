
#include "Raytracer.h"
#include "SceneLoader.h"
#include "TriangleMesh.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

using namespace std;

int main(int argc, char *argv[]) {

    mkdir("output", 0755);
    mkdir("output/book_spin", 0755);

    // Load scene once — mesh is built once and reused every frame
    Scene scene = SceneLoader::load("scenes/book_test.json");

    // Find the mesh object (index 1 — after the floor quad)
    Object* mesh = nullptr;
    for (auto* obj : scene.object_ptrs) {
        if (dynamic_cast<TriangleMesh*>(obj)) { mesh = obj; break; }
    }

    int total_frames = 120;
    double degrees_per_frame = 2.0 * M_PI / total_frames;

    cout << "Total frames: " << total_frames << endl;

    Vect campos(0, 2.4, -6.0);
    Vect lookat(0, 0.5, 0);

    for (int frame = 0; frame < total_frames; frame++) {
        // Reset rotation and apply exact angle for this frame
        if (mesh) {
            mesh->rotation = Matrix4x4();
            mesh->inverse  = Matrix4x4();
            mesh->rotateY(degrees_per_frame * frame);
        }

        ostringstream ss;
        ss << "output/book_spin/frame" << setw(4) << setfill('0') << frame << ".png";

        cout << "Frame " << frame << "/" << total_frames << endl;

        Raytracer tracer;
        tracer.generate(scene.object_ptrs, scene.light_ptrs, ss.str(),
                        scene.aa, campos, lookat, true,
                        scene.tonemap, scene.gamma, scene.ambient);
    }

    cout << "Done." << endl;
    return 0;
}
