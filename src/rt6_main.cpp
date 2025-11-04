#include <fstream>
#include <random>
#include "camera.h"
#include "scene.h"
#include "material.h"
#include "light.h"
#include "color.h"

int main(){
    const int image_width  = 1080;
    const int image_height = static_cast<int>(image_width / (16.0/9.0));
    const int max_depth    = 8;
    const int spp_direct   = 16;     // MC samples per pixel for direct light

    Camera cam;
    Scene scene;

    // --- Materials ---
    Material redLambert  { MatType::LAMBERT, Color(0.9, 0.2, 0.2) };
    Material greenLambert{ MatType::LAMBERT, Color(0.2, 0.8, 0.2) };
    Material greyLambert { MatType::LAMBERT, Color(0.8, 0.8, 0.8) };
    Material mirror      { MatType::MIRROR , Color(0,0,0) };

    // --- Geometry (+x is forward) ---
    scene.spheres.emplace_back(Vec3(3.0, -0.4, -0.25), 0.5, redLambert);
    scene.spheres.emplace_back(Vec3(4.6,  0.7, -0.10), 0.6, mirror);

    // Ground z = -0.75
    scene.planes.push_back({ Plane(Vec3(0,0,-0.75), Vec3(0,0,1)), greyLambert });

    // Optional: left wall at y = -1.2 (Lambert green)
    scene.planes.push_back({ Plane(Vec3(0,-1.2,0), Vec3(0,1,0)), greenLambert });

    // --- Rectangular roof light (z = +5, shining downward) ---
    // Square 4x4 centered around (x=4, y=0, z=5); edges along +y and +x.
    Vec3 v0 = Vec3(2, -2, 5);  // corner
    Vec3 e1 = Vec3(0, 4, 0);   // along +y
    Vec3 e2 = Vec3(4, 0, 0);   // along +x
    Vec3 nL = Vec3(0, 0, -1);  // toward the room
    scene.lights.emplace_back(v0, e1, e2, nL, Color(1,1,1));  // Le=(1,1,1)

    std::ofstream out("image6.ppm", std::ios::binary);
    out << "P6\n" << image_width << " " << image_height << "\n255\n";

    // simple RNG per render
    std::random_device rd;
    std::mt19937_64 rng(rd());

    for (int j = image_height - 1; j >= 0; --j){
        for (int i = 0; i < image_width; ++i){
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);

            Ray r = cam.get_ray(u, v);
            Color c = scene.shade(r, max_depth, spp_direct, rng);

            uint8_t R,G,B;
            to_u8(c, R, G, B, 1.0);
            out.put(char(R)).put(char(G)).put(char(B));
        }
    }
    return 0;
}
