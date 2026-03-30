#ifndef _SCENELOADER_H
#define _SCENELOADER_H

#include "nlohmann_json.hpp"
#include "Raytracer.h"
#include "Light.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Rectangle.h"
#include "Pyramid.h"
#include "Quad.h"
#include "TriangleMesh.h"
#include "Texture.h"
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

using json = nlohmann::json;

// Owns all heap-allocated scene objects and lights for a single frame.
struct Scene {
    std::vector<std::unique_ptr<Object>>       objects;
    std::vector<std::unique_ptr<Source>>       lights;
    std::vector<std::unique_ptr<Texture>>      textures;      // keeps textures alive
    std::vector<std::unique_ptr<NormalMapTexture>> normalMaps; // keeps normal maps alive

    // Raw pointers passed to Raytracer::generate
    std::vector<Object*> object_ptrs;
    std::vector<Source*> light_ptrs;

    Vect    campos   { 0, 5, -10     };
    Vect    lookat   { 0, 0,  0      };
    int     aa       { 1             };
    ToneMap tonemap  { ToneMap::None };
    double  gamma    { 1.0           };
    double  ambient  { 0.35          };
};

class SceneLoader {
public:
    static Scene load(const std::string& path) {
        std::ifstream f(path);
        if (!f) throw std::runtime_error("Cannot open scene file: " + path);
        json j = json::parse(f, nullptr, true, true); // allow comments
        return parse(j);
    }

private:
    static Vect parseVect(const json& j) {
        return Vect(j[0].get<double>(), j[1].get<double>(), j[2].get<double>());
    }

    static Color parseColor(const json& obj, const std::string& key, double specularity = 0.0) {
        const json& c = obj.at(key);
        return Color(c[0].get<double>(), c[1].get<double>(), c[2].get<double>(), specularity);
    }

    static std::unique_ptr<Texture> parseTexture(const json& t) {
        std::string type = t.at("type").get<std::string>();

        if (type == "checkerboard") {
            auto ca = t.at("color_a"); auto cb = t.at("color_b");
            double spec_a = t.value("specularity_a", 0.0);
            double spec_b = t.value("specularity_b", 0.0);
            Color a(ca[0], ca[1], ca[2], spec_a);
            Color b(cb[0], cb[1], cb[2], spec_b);
            double scale = t.value("scale", 1.0);
            return std::make_unique<CheckerboardTexture>(a, b, scale);
        }

        if (type == "noise") {
            double scale   = t.value("scale",     1.0);
            int    seed    = t.value("seed",       1337);
            float  freq    = t.value("frequency",  1.0f);
            int    octaves = t.value("octaves",    4);

            Color low(0,0,0,0), high(1,1,1,0);
            if (t.contains("color_low"))  { auto c=t["color_low"];  low  = Color(c[0],c[1],c[2],0); }
            if (t.contains("color_high")) { auto c=t["color_high"]; high = Color(c[0],c[1],c[2],0); }

            auto tex = std::make_unique<NoiseTexture>(scale, seed, low, high);

            std::string noise_type = t.value("noise_type", "simplex_fractal");
            if      (noise_type == "simplex")         tex->setNoiseType(FastNoise::Simplex);
            else if (noise_type == "simplex_fractal") tex->setNoiseType(FastNoise::SimplexFractal);
            else if (noise_type == "value")           tex->setNoiseType(FastNoise::Value);
            else if (noise_type == "cellular")        tex->setNoiseType(FastNoise::Cellular);
            else throw std::runtime_error("Unknown noise_type: " + noise_type);

            tex->setFrequency(freq);
            tex->setOctaves(octaves);
            return tex;
        }

        if (type == "image") {
            std::string imgpath = t.at("path").get<std::string>();
            return std::make_unique<ImageTexture>(imgpath);
        }

        throw std::runtime_error("Unknown texture type: " + type);
    }

    static void applyTransform(Object* obj, const json& transforms) {
        for (const auto& op : transforms) {
            if (op.contains("rotateX"))  obj->rotateX(op["rotateX"].get<double>());
            if (op.contains("rotateY"))  obj->rotateY(op["rotateY"].get<double>());
            if (op.contains("rotateZ"))  obj->rotateZ(op["rotateZ"].get<double>());
            if (op.contains("position")) obj->position = parseVect(op["position"]);
            if (op.contains("translate")) {
                Vect v = parseVect(op["translate"]);
                obj->translate(v);
            }
        }
    }

    static void finalizeObject(Object* obj, double shininess, double transparency, double ior, Color emission) {
        obj->shininess    = shininess;
        obj->ior          = ior;
        obj->transparency = transparency;
        obj->emission     = emission;
    }

    static void attachTexture(Object* obj, const json& jobj, Scene& scene) {
        if (!jobj.contains("texture") || jobj["texture"].is_null()) return;
        auto tex = parseTexture(jobj["texture"]);
        obj->texture = tex.get();
        scene.textures.push_back(std::move(tex));
    }

    static void attachNormalMap(Object* obj, const json& jobj, Scene& scene) {
        if (!jobj.contains("normal_map")) return;
        std::string path = jobj["normal_map"].get<std::string>();
        auto nm = std::make_unique<NormalMapTexture>(path);
        obj->normalMap = nm.get();
        scene.normalMaps.push_back(std::move(nm));
    }

    static Scene parse(const json& j) {
        Scene scene;

        // Camera
        if (j.contains("ambient")) scene.ambient = j["ambient"].get<double>();

        if (j.contains("camera")) {
            const auto& cam = j["camera"];
            if (cam.contains("position"))    scene.campos  = parseVect(cam["position"]);
            if (cam.contains("look_at"))     scene.lookat  = parseVect(cam["look_at"]);
            if (cam.contains("aa"))          scene.aa      = cam["aa"].get<int>();
            if (cam.contains("gamma"))       scene.gamma   = cam["gamma"].get<double>();
            if (cam.contains("tonemapping")) {
                std::string tm = cam["tonemapping"].get<std::string>();
                if      (tm == "reinhard") scene.tonemap = ToneMap::Reinhard;
                else if (tm == "aces")     scene.tonemap = ToneMap::ACES;
                else if (tm == "none")     scene.tonemap = ToneMap::None;
                else throw std::runtime_error("Unknown tonemapping: " + tm);
            }
        }

        // Lights
        for (const auto& l : j.value("lights", json::array())) {
            Vect  pos    = parseVect(l.at("position"));
            auto  c      = l.at("color");
            Color color(c[0], c[1], c[2], 0);
            double radius = l.value("radius", 0.0);
            auto light = std::make_unique<Light>(pos, color, radius);
            light->intensity = l.value("intensity", 0.0);
            scene.light_ptrs.push_back(light.get());
            scene.lights.push_back(std::move(light));
        }

        // Objects
        for (const auto& o : j.value("objects", json::array())) {
            std::string type = o.at("type").get<std::string>();
            double spec         = o.value("specularity",  0.0);
            double shininess    = o.value("shininess",    0.0);
            double transparency = o.value("transparency", 0.0);
            double ior          = o.value("ior",          0.0);
            Color emission(0, 0, 0, 0);
            if (o.contains("emission")) {
                const auto& e = o["emission"];
                emission = Color(e[0].get<double>(), e[1].get<double>(), e[2].get<double>(), 0);
            }

            if (type == "sphere") {
                Vect center = parseVect(o.at("center"));
                double radius = o.at("radius").get<double>();
                Color color = parseColor(o, "color", spec);
                auto obj = std::make_unique<Sphere>(center, radius, color);
                attachTexture(obj.get(), o, scene);
                attachNormalMap(obj.get(), o, scene);
                if (o.contains("transform")) applyTransform(obj.get(), o["transform"]);
                finalizeObject(obj.get(), shininess, transparency, ior, emission);
                scene.object_ptrs.push_back(obj.get());
                scene.objects.push_back(std::move(obj));
            }
            else if (type == "plane") {
                Vect normal   = parseVect(o.at("normal"));
                double dist   = o.at("distance").get<double>();
                Color color   = parseColor(o, "color", spec);
                auto obj = std::make_unique<Plane>(normal, dist, color);
                attachTexture(obj.get(), o, scene);
                finalizeObject(obj.get(), shininess, transparency, ior, emission);
                scene.object_ptrs.push_back(obj.get());
                scene.objects.push_back(std::move(obj));
            }
            else if (type == "cone") {
                double zmin = o.at("z_min").get<double>();
                double zmax = o.at("z_max").get<double>();
                Color color = parseColor(o, "color", spec);
                auto obj = std::make_unique<Cone>(zmin, zmax, color);
                attachTexture(obj.get(), o, scene);
                if (o.contains("transform")) applyTransform(obj.get(), o["transform"]);
                finalizeObject(obj.get(), shininess, transparency, ior, emission);
                scene.object_ptrs.push_back(obj.get());
                scene.objects.push_back(std::move(obj));
            }
            else if (type == "cylinder") {
                double ymin   = o.at("y_min").get<double>();
                double ymax   = o.at("y_max").get<double>();
                double radius = o.at("radius").get<double>();
                Color color   = parseColor(o, "color", spec);
                auto obj = std::make_unique<Cylinder>(ymin, ymax, radius, color);
                attachTexture(obj.get(), o, scene);
                if (o.contains("transform")) applyTransform(obj.get(), o["transform"]);
                finalizeObject(obj.get(), shininess, transparency, ior, emission);
                scene.object_ptrs.push_back(obj.get());
                scene.objects.push_back(std::move(obj));
            }
            else if (type == "rectangle") {
                Vect center  = parseVect(o.at("center"));
                double w     = o.at("width").get<double>();
                double h     = o.at("height").get<double>();
                double d     = o.at("depth").get<double>();
                Color color  = parseColor(o, "color", spec);
                auto obj = std::make_unique<Rectangle>(center, w, h, d, color);
                attachTexture(obj.get(), o, scene);
                if (o.contains("transform")) applyTransform(obj.get(), o["transform"]);
                finalizeObject(obj.get(), shininess, transparency, ior, emission);
                scene.object_ptrs.push_back(obj.get());
                scene.objects.push_back(std::move(obj));
            }
            else if (type == "pyramid") {
                Vect base    = parseVect(o.at("base"));
                int sides    = o.at("sides").get<int>();
                double radius= o.at("radius").get<double>();
                double height= o.at("height").get<double>();
                Color color  = parseColor(o, "color", spec);
                auto obj = std::make_unique<Pyramid>(base, sides, radius, height, color);
                attachTexture(obj.get(), o, scene);
                if (o.contains("transform")) applyTransform(obj.get(), o["transform"]);
                finalizeObject(obj.get(), shininess, transparency, ior, emission);
                scene.object_ptrs.push_back(obj.get());
                scene.objects.push_back(std::move(obj));
            }
            else if (type == "quad") {
                Vect center  = parseVect(o.at("center"));
                double width = o.at("width").get<double>();
                double depth = o.at("depth").get<double>();
                Color color  = parseColor(o, "color", spec);
                auto obj = std::make_unique<Quad>(center, width, depth, color);
                attachTexture(obj.get(), o, scene);
                if (o.contains("transform")) applyTransform(obj.get(), o["transform"]);
                finalizeObject(obj.get(), shininess, transparency, ior, emission);
                scene.object_ptrs.push_back(obj.get());
                scene.objects.push_back(std::move(obj));
            }
            else if (type == "mesh") {
                std::string meshpath = o.at("path").get<std::string>();
                Color color = parseColor(o, "color", spec);
                auto obj = std::make_unique<TriangleMesh>(meshpath, color);
                attachTexture(obj.get(), o, scene);
                attachNormalMap(obj.get(), o, scene);
                if (o.contains("transform")) applyTransform(obj.get(), o["transform"]);
                finalizeObject(obj.get(), shininess, transparency, ior, emission);
                scene.object_ptrs.push_back(obj.get());
                scene.objects.push_back(std::move(obj));
            }
            else {
                throw std::runtime_error("Unknown object type: " + type);
            }
        }

        return scene;
    }
};

#endif
