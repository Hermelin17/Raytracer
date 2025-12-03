#include <fstream>
#include <random>
#include "camera.h"
#include "scene.h"
#include "material.h"
#include "rectangle.h"
#include "triangle.h"
#include "light.h"
#include "color.h"
#include <thread>
#include <atomic>
#include <iostream>



static void build_hex_room(Scene& S){
    // materials
    Material wallLambert { MatType::LAMBERT, Color(0.7,0.7,0.7) };
    Material floorLambert{ MatType::LAMBERT, Color(0.7,0.7,0.7) };
    Material roofLambert { MatType::LAMBERT, Color(0.7,0.7,0.7) };
    
    

    // XY vertices (closed polygon)
    Vec3 P[6] = { Vec3(0, 6, 0), Vec3(10, 6, 0), Vec3(13, 0, 0),
                  Vec3(10,-6, 0), Vec3(0,-6, 0), Vec3(-3, 0, 0) };

    // 6 wall rectangles: v0 at z=-5; e1 = +z(10); e2 = edge along XY
    for (int i=0;i<6;++i){
        Vec3 a = P[i], b = P[(i+1)%6];
        Vec3 v0(a.x, a.y, -5);
        Vec3 e1(0,0,10);
        Vec3 e2(b.x - a.x, b.y - a.y, 0);
        S.rects.push_back({ Rectangle(v0, e1, e2), wallLambert });
    }

    // Floor (z = -5) and Roof (z = +5)
    // Central rectangle between (0,-6) and (10,6) in XY
    auto add_floor_rect = [&](double z, const Material& m){
        Vec3 v0(0,-6,z);
        Vec3 e1(10, 0, 0);  // along +x
        Vec3 e2( 0,12, 0);  // along +y
        S.rects.push_back({ Rectangle(v0, e1, e2), m });
    };
    auto add_floor_tris = [&](double z, const Material& m){
        // Left wedge: (-3,0)-(0,6)-(0,-6)
        S.tris.push_back({ Triangle(Vec3(-3,0,z), Vec3(0,6,z),  Vec3(0,-6,z)), m });
        // Right wedge: (10,6)-(13,0)-(10,-6)
        S.tris.push_back({ Triangle(Vec3(10,6,z), Vec3(13,0,z), Vec3(10,-6,z)), m });
    };

    add_floor_rect(-5, floorLambert);
    add_floor_tris(-5, floorLambert);
    add_floor_rect(+5, roofLambert);
    add_floor_tris(+5, roofLambert); 
}

int main(){
    const int W =400, H = 400;     // Lecture suggests ~800x800
    const int spp = 20, ls = 10, depth = 20;

    Camera cam;
    Scene scene;
    build_hex_room(scene);

    Material lamp { MatType::EMISSIVE, Color(0,0,0), Color(1.5,1.5,1.5) }; // Le = (1,1,1)
    Material red{ MatType::LAMBERT, Color(0.9,0.2,0.2) };
    Material mirror { MatType::MIRROR, Color(0,0,0) };
    Material blueLambert { MatType::LAMBERT, Color(0.2, 0.2, 0.9) };
    Material greenLambert  { MatType::LAMBERT, Color(0.2, 0.9, 0.2) };

    scene.rects[0].mat = greenLambert;   // right wall (y=+6)
    scene.rects[3].mat = blueLambert;  // left wall  (y=-6)
    scene.rects[1].mat = mirror;  // left wall  (y=-6)
   
    //Spheres
    scene.spheres.emplace_back(Vec3(5.0, 0.0, -3), 0.8, red);
    scene.spheres.emplace_back(Vec3(5.0, 2, -3), 0.65, mirror);

    // Roof area light at z=+5 facing downward (same 4x4 as before, centered near x~4,y~0)
    Vec3 v0 = Vec3(2,-2,5), e1 = Vec3(0,4,0), e2 = Vec3(4,0,0), nL = Vec3(0,0,-1);
    scene.lights.emplace_back(v0, e1, e2, nL, Color(1.3,1.3,1.3));

    {
    Vec3 v0g = Vec3(2, -2, 4.9);   // corner
    Vec3 e1g = Vec3(0, 4, 0);      // along +y
    Vec3 e2g = Vec3(4, 0, 0);      // along +x
    Scene::RectGeom lampRect{ Rectangle(v0g, e1g, e2g), lamp };
    scene.rects.push_back(lampRect);
    }

    // --- Add a small tetrahedron (polygonal object) ---
    Material yellowPoly{ MatType::LAMBERT, Color(0.9,0.9,0.2) };

    // vertices (centered near x≈4.3, y≈-1.0, z≈-2.3)
    Vec3 A(5.3, -3, -4);
    Vec3 B(5.6, -1.5, -4);
    Vec3 C(5.3, -3, 0.3);
    Vec3 D(5.3, -2.2, -4);

    // 4 faces (triangles)
    scene.tris.push_back({ Triangle(A, B, C), yellowPoly });
    scene.tris.push_back({ Triangle(A, C, D), yellowPoly });
    scene.tris.push_back({ Triangle(A, D, B), yellowPoly });
    scene.tris.push_back({ Triangle(B, D, C), yellowPoly });

    // std::ofstream out("room.ppm", std::ios::binary);
    // out << "P6\n" << W << " " << H << "\n255\n";

    std::mt19937_64 rng(1);
    std::uniform_real_distribution<double> U(0.0,1.0);

    std::vector<uint8_t> pixels(W * H * 3, 0);
    std::atomic<int> next_row{0};
    int num_threads = std::max(1u, std::thread::hardware_concurrency());

    auto worker = [&](uint64_t seed_base){
        std::mt19937_64 rng(seed_base);
        std::uniform_real_distribution<double> U(0.0,1.0);

        while (true) {
            int j = next_row.fetch_add(1);
            if (j >= H) break;

            for (int i = 0; i < W; ++i){
                Color acc(0,0,0);
                for (int s=0;s<spp;++s){
                    double u = (i + U(rng)) / (W - 1);
                    double v = (j + U(rng)) / (H - 1);
                    Ray r = cam.get_ray(u, v);
                    Color c = scene.shade_path(r, depth, ls, rng);
                    double m = std::max({c.r,c.g,c.b});
                    if (m>10.0) c = c * (10.0/m);
                    acc = acc + c;
                }
                acc = acc * (1.0/spp);

                uint8_t R,G,B; to_u8(acc, R,G,B, 1.0);
                size_t idx = ((H-1-j)*W + i) * 3; // flip vertically for PPM
                pixels[idx+0] = R; pixels[idx+1] = G; pixels[idx+2] = B;
            }
        }
    };

    // spawn threads
    std::vector<std::thread> pool;
    for (int t = 0; t < num_threads; ++t)
        pool.emplace_back(worker, 1234u + t * 1337u);

    // --- progress monitor (main thread) ---
    auto start = std::chrono::steady_clock::now();
    int total_rows = H;
    while (true) {
        int done = next_row.load();
        double progress = double(done) / total_rows;
        if (progress > 0.0) {
            auto now = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(now - start).count();
            double eta = elapsed * (1.0 / progress - 1.0);
            if(eta > 60)
            {
                eta /= 60;
                std::cerr << "\rRendering: "
                << int(progress * 100) << "% | elapsed "
                << int(elapsed) << "s | ETA "
                << int(eta) << " min" << std::flush;
            }
            else
            {
            std::cerr << "\rRendering: "
                << int(progress * 100) << "% | elapsed "
                << int(elapsed) << "s | ETA "
                << int(eta) << "s   " << std::flush;
            }

        }
        if (done >= total_rows) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    // Wait for all threads to finish
    for (auto &th : pool) th.join();

    std::cerr << "\nRender finished in "
        << std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count()
        << " s\n";

    // write PPM once
    std::ofstream out("room.ppm", std::ios::binary);
    out << "P6\n" << W << " " << H << "\n255\n";
    out.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());

}
