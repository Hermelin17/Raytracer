#include <fstream>
#include "camera.h"
#include "scene.h"
#include "color.h"

int main(){
    const int image_width = 600;
    const int image_height = static_cast<int>(image_width / (16.0/9.0));

    Camera cam;
    Scene scene;

    // objects: sphere and a ground plane at z = -0.75
    scene.spheres.emplace_back(Vec3(3, 0, -0.25), 0.75, Color(0.9, 0.2, 0.2)); // red sphere
    scene.planes.push_back({ Scene::PlaneGeom{ Plane(Vec3(0,0,-0.75), Vec3(0,0,1)), Color(0.8,0.8,0.8) } });

    std::ofstream out("image4.ppm", std::ios::binary);
    out << "P6\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j){
        for (int i = 0; i < image_width; ++i){
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            Ray r = cam.get_ray(u, v);

            auto hit = scene.trace_first(r, 1e-4, 1e9);
            Color c;
            if (hit.hit){
                c = scene.shade_lambert(hit);
            } else {
                c = scene.background(r);
            }

            uint8_t R,G,B;
            to_u8(c, R, G, B, 1.0);
            out.put(char(R)).put(char(G)).put(char(B));
        }
    }
    return 0;
}
