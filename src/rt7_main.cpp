#include <fstream>
#include <random>
#include <chrono>
#include <iostream>
#include <cstring>
#include "camera.h"
#include "scene.h"
#include "material.h"
#include "light.h"
#include "color.h"

static int  argi(const char* name, int def, int argc, char** argv){
    for (int k=1; k<argc-1; ++k) if (!std::strcmp(argv[k], name)) return std::atoi(argv[k+1]);
    return def;
}
static double argd(const char* name, double def, int argc, char** argv){
    for (int k=1; k<argc-1; ++k) if (!std::strcmp(argv[k], name)) return std::atof(argv[k+1]);
    return def;
}
static const char* args(const char* name, const char* def, int argc, char** argv){
    for (int k=1; k<argc-1; ++k) if (!std::strcmp(argv[k], name)) return argv[k+1];
    return def;
}

int main(int argc, char** argv){
    // --- parameters with sensible defaults ---
    int image_width  = argi("--w",   800,  argc, argv);
    double aspect    = argd("--ar",  16.0/9.0, argc, argv);
    int image_height = int(image_width / aspect + 0.5);
    int spp          = argi("--spp", 128,  argc, argv);   // pixel samples
    int direct_spp   = argi("--ls",  16,   argc, argv);   // light samples per hit
    int max_depth    = argi("--d",   8,    argc, argv);
    uint64_t seed    = (uint64_t)argi("--seed", 1, argc, argv); // 0 => time-based
    const char* outName = args("--out", "image7.ppm", argc, argv);

    std::cout << "w="<<image_width<<" h="<<image_height
              << " spp="<<spp<<" ls="<<direct_spp<<" depth="<<max_depth
              << " seed="<<seed<<" out="<<outName<<"\n";

    if (seed==0){
        seed = (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }

    Camera cam;
    Scene scene;

    // --- Materials ---
    Material redLambert  { MatType::LAMBERT, Color(0.9, 0.2, 0.2) };
    Material greenLambert{ MatType::LAMBERT, Color(0.25, 0.6, 0.25) };
    Material greyLambert { MatType::LAMBERT, Color(0.8, 0.8, 0.8) };
    Material mirror      { MatType::MIRROR , Color(0,0,0) };

    // --- Geometry (+x forward) ---
    scene.spheres.emplace_back(Vec3(3.0, -0.4, -0.25), 0.5, redLambert);
    scene.spheres.emplace_back(Vec3(4.6,  0.7, -0.10), 0.6, mirror);

    scene.planes.push_back({ Plane(Vec3(0,0,-0.75), Vec3(0,0,1)), greyLambert });
    scene.planes.push_back({ Plane(Vec3(0,-1.2,0), Vec3(0,1,0)), greenLambert });
    // comment next line to remove mirror wall:
    scene.planes.push_back({ Plane(Vec3(0, 1.0,0), Vec3(0,-1,0)), mirror });

    // --- Rectangular roof light (z=5), 4x4 square centered around (x≈4,y=0) ---
    Vec3 v0 = Vec3(2, -2, 5), e1 = Vec3(0, 4, 0), e2 = Vec3(4, 0, 0), nL = Vec3(0,0,-1);
    scene.lights.emplace_back(v0, e1, e2, nL, Color(1,1,1));

    std::ofstream out(outName, std::ios::binary);
    out << "P6\n" << image_width << " " << image_height << "\n255\n";

    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> U(0.0, 1.0);

    auto t0 = std::chrono::high_resolution_clock::now();

    for (int j = image_height - 1; j >= 0; --j){
        for (int i = 0; i < image_width; ++i){
            Color accum(0,0,0);
            for (int s = 0; s < spp; ++s){
                double u = (i + U(rng)) / (image_width  - 1);
                double v = (j + U(rng)) / (image_height - 1);
                Ray r = cam.get_ray(u, v);

                Color c = scene.shade_path(r, max_depth, direct_spp, rng);

                // optional firefly clamp (simple, conservative)
                double m = std::max({c.r,c.g,c.b});
                if (m > 10.0) c = c * (10.0 / m);

                accum = accum + c;
            }
            accum = accum * (1.0 / spp);

            uint8_t R,G,B;
            to_u8(accum, R, G, B, 1.0);
            out.put(char(R)).put(char(G)).put(char(B));
        }
        if ((image_height - j) % 32 == 0) std::cerr << "." << std::flush; // tiny progress
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    double sec = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "\nDone in " << sec << " s\n";
    return 0;
}
