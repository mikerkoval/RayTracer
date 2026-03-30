#include "SceneLoader.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: render <scene.json> <output.png>\n";
        return 1;
    }

    Scene scene;
    try {
        scene = SceneLoader::load(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error loading scene: " << e.what() << "\n";
        return 1;
    }

    Raytracer tracer;
    tracer.generate(scene.object_ptrs, scene.light_ptrs, argv[2], scene.aa,
                    scene.campos, scene.lookat, true, scene.tonemap, scene.gamma, scene.ambient);
    return 0;
}
