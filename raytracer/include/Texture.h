#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "Color.h"
#include "FastNoise.h"
#include "Magick++.h"
#include <cmath>
#include <string>

class Texture {
public:
    virtual ~Texture() {}
    virtual Color getColor(double u, double v) = 0;
};

class CheckerboardTexture : public Texture {
    Color color_a;
    Color color_b;
    double scale;
public:
    CheckerboardTexture(Color a, Color b, double scale = 1.0)
        : color_a(a), color_b(b), scale(scale) {}

    virtual Color getColor(double u, double v) {
        double su = u / scale;
        double sv = v / scale;
        int cu = (int)floor(su);
        int cv = (int)floor(sv);

        // Fractional position within cell
        double fu = su - cu;
        double fv = sv - cv;

        // Smooth the step: blend each axis independently around 0.5
        const double w = 0.02;
        double bu = smoothstep(0.5 - w, 0.5 + w, fu);
        double bv = smoothstep(0.5 - w, 0.5 + w, fv);

        // Each axis flips at 0.5; XOR with parity gives correct checker
        int parity = (cu + cv) % 2;
        double au = (parity & 1) ? (1.0 - bu) : bu;
        double t = au * (1.0 - bv) + (1.0 - au) * bv;

        return Color(
            color_a.getRed()          * (1.0 - t) + color_b.getRed()          * t,
            color_a.getGreen()        * (1.0 - t) + color_b.getGreen()        * t,
            color_a.getBlue()         * (1.0 - t) + color_b.getBlue()         * t,
            color_a.getSpecularity()  * (1.0 - t) + color_b.getSpecularity()  * t
        );
    }

private:
    static double smoothstep(double edge0, double edge1, double x) {
        double t = (x - edge0) / (edge1 - edge0);
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;
        return t * t * (3.0 - 2.0 * t);
    }
};

class NoiseTexture : public Texture {
    FastNoise noise;
    double scale;
    Color color_low;
    Color color_high;
public:
    NoiseTexture(double scale = 1.0, int seed = 1337,
                 Color low  = Color(0.0, 0.0, 0.0, 0.0),
                 Color high = Color(1.0, 1.0, 1.0, 0.0))
        : scale(scale), color_low(low), color_high(high)
    {
        noise.SetSeed(seed);
        noise.SetNoiseType(FastNoise::SimplexFractal);
        noise.SetFrequency(1.0f);
        noise.SetFractalOctaves(4);
    }

    void setNoiseType(FastNoise::NoiseType type) { noise.SetNoiseType(type); }
    void setFrequency(float freq)                { noise.SetFrequency(freq); }
    void setOctaves(int octaves)                 { noise.SetFractalOctaves(octaves); }

    virtual Color getColor(double u, double v) {
        float n = noise.GetNoise((float)(u * scale), (float)(v * scale));
        double t = (n + 1.0) * 0.5;
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        return Color(
            color_low.getRed()   * (1.0 - t) + color_high.getRed()   * t,
            color_low.getGreen() * (1.0 - t) + color_high.getGreen() * t,
            color_low.getBlue()  * (1.0 - t) + color_high.getBlue()  * t,
            0.0
        );
    }
};

class NormalMapTexture {
    int width;
    int height;
    std::vector<unsigned char> pixels;
public:
    NormalMapTexture(const std::string& path) {
        Magick::Image image;
        image.read(path);
        image.depth(8);
        width  = image.columns();
        height = image.rows();
        pixels.resize(width * height * 3);
        image.write(0, 0, width, height, "RGB", Magick::CharPixel, pixels.data());
    }

    // Returns tangent-space normal in [-1,1]^3
    Vect sample(double u, double v) {
        u = u - floor(u);
        v = v - floor(v);
        int x = (int)(u * width);
        int y = (int)(v * height);
        if (x >= width)  x = width  - 1;
        if (y >= height) y = height - 1;
        int i = (y * width + x) * 3;
        double nx = pixels[i + 0] / 255.0 * 2.0 - 1.0;
        double ny = pixels[i + 1] / 255.0 * 2.0 - 1.0;
        double nz = pixels[i + 2] / 255.0 * 2.0 - 1.0;
        return Vect(nx, ny, nz);
    }
};

class ImageTexture : public Texture {
    int width;
    int height;
    std::vector<unsigned char> pixels;  // cached 8-bit RGB
public:
    ImageTexture(const std::string& path) {
        Magick::Image image;
        image.read(path);
        image.depth(8);
        width  = image.columns();
        height = image.rows();
        pixels.resize(width * height * 3);
        image.write(0, 0, width, height, "RGB", Magick::CharPixel, pixels.data());
    }

    // u, v expected in [0, 1]
    virtual Color getColor(double u, double v) {
        u = u - floor(u);
        v = v - floor(v);
        int x = (int)(u * width);
        int y = (int)(v * height);
        if (x >= width)  x = width  - 1;
        if (y >= height) y = height - 1;
        int i = (y * width + x) * 3;
        return Color(
            pixels[i + 0] / 255.0,
            pixels[i + 1] / 255.0,
            pixels[i + 2] / 255.0,
            0.0
        );
    }
};

#endif
