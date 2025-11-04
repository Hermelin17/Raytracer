#include <fstream>
#include "camera.h"
#include "scene.h"
#include "material.h"
#include "color.h"

int main(){
    const int image_width  = 700;
    const int image_height = static_cast<int>(image_width / (16.0/9.0));
    const int max_depth    = 8;   // reflection depth

    Camera cam;
    Scene scene;

    // Materials
    Material redLambert{ MatType::LAMBERT, Color(0.9, 0.2, 0.2) };
    Material greyLambert{ MatType::LAMBERT, Color(0.8, 0.8, 0.8) };
    Material mirror{ MatType::MIRROR, Color(0,0,0) };

    // Objects (remember: +x is forward)
    scene.spheres.emplace_back(Vec3(3.0, -0.4, -0.25), 0.5, redLambert); // diffuse red
    scene.spheres.emplace_back(Vec3(4.5,  0.6, -0.10), 0.6, mirror);     // mirror sphere

    // Ground plane z = -0.75 (Lambert)
    scene.planes.push_back({ Plane(Vec3(0,0,-0.75), Vec3(0,0,1)), greyLambert });

    // Optional: vertical mirror wall at y = +1.0 (normal points -y)
    scene.planes.push_back({ Plane(Vec3(0, 1.0, 0), Vec3(0,-1,0)), mirror });

    std::ofstream out("image5.ppm", std::ios::binary);
    out << "P6\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j){
        for (int i = 0; i < image_width; ++i){
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            Ray r = cam.get_ray(u, v);

            Color c = scene.shade(r, max_depth);

            uint8_t R,G,B;
            to_u8(c, R, G, B, 1.0);
            out.put(char(R)).put(char(G)).put(char(B));
        }
    }
    return 0;
}
