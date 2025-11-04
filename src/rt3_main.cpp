#include <fstream>
#include "camera.h"
#include "scene.h"
#include "color.h"

static Color shade_normal(const Hit& h){
    // map normal [-1,1] -> [0,1]
    return Color(0.5*(h.n.x+1.0), 0.5*(h.n.y+1.0), 0.5*(h.n.z+1.0));
}

int main(){
    const int image_width = 500;
    const int image_height = static_cast<int>(image_width / (16.0/9.0));

    Camera cam;
    Scene scene;
    // our camera faces +x, so place a sphere in front at x=3
    scene.spheres.emplace_back(Vec3(3, 0, 0), 0.75);

    std::ofstream out("image3.ppm", std::ios::binary);
    out << "P6\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j){
        for (int i = 0; i < image_width; ++i){
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            Ray r = cam.get_ray(u, v);

            Hit rec;
            Color c;
            if (scene.trace_first(r, 1e-4, 1e9, rec)){
                c = shade_normal(rec);
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
