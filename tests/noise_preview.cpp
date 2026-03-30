#include "FastNoise.h"
#include "Magick++.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

static void render_noise(FastNoise& fn, const string& outpath, int width, int height) {
    vector<unsigned char> buf(width * height * 3);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = (float)x / width;
            float ny = (float)y / height;
            float n = fn.GetNoise(nx, ny);
            double t = (n + 1.0) * 0.5;
            if (t < 0) t = 0;
            if (t > 1) t = 1;
            unsigned char v = (unsigned char)(t * 255);
            int i = (y * width + x) * 3;
            buf[i + 0] = v;
            buf[i + 1] = v;
            buf[i + 2] = v;
        }
    }
    Magick::Image img;
    img.read(width, height, "RGB", Magick::CharPixel, buf.data());
    img.write(outpath);
    cout << "Saved " << outpath << "\n";
}

int main() {
    const int W = 512, H = 512;
    Magick::InitializeMagick(nullptr);

    {
        FastNoise fn(1337);
        fn.SetNoiseType(FastNoise::Simplex);
        fn.SetFrequency(4.0f);
        render_noise(fn, "tests/references/noise_simplex.png", W, H);
    }
    {
        FastNoise fn(1337);
        fn.SetNoiseType(FastNoise::SimplexFractal);
        fn.SetFrequency(2.0f);
        fn.SetFractalOctaves(5);
        render_noise(fn, "tests/references/noise_simplex_fractal.png", W, H);
    }
    {
        FastNoise fn(1337);
        fn.SetNoiseType(FastNoise::Value);
        fn.SetFrequency(4.0f);
        render_noise(fn, "tests/references/noise_value.png", W, H);
    }
    {
        FastNoise fn(1337);
        fn.SetNoiseType(FastNoise::Cellular);
        fn.SetFrequency(4.0f);
        render_noise(fn, "tests/references/noise_cellular.png", W, H);
    }

    return 0;
}
