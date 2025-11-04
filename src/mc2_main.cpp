#include <fstream>
#include "camera.h"
#include "scene.h"
#include "color.h"

int main() {
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / (16.0 / 9.0));
    const int samples_per_pixel = 1;

    Camera cam;
    Scene scene;

    std::ofstream out("image2.ppm", std::ios::binary);
    out << "P6\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            Ray r = cam.get_ray(u, v);
            Color c = scene.background(r);
            uint8_t R, G, B;
            to_u8(c, R, G, B, 1.0);
            out.put(char(R)).put(char(G)).put(char(B));
        }
    }
    return 0;
}
