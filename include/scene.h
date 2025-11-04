#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include "color.h"
#include "ray.h"
#include "hit.h"
#include "material.h"
#include "rectangle.h"
#include "triangle.h"
#include "sphere.h"
#include "light.h"

struct Scene {
    struct RectGeom { Rectangle R; Material mat; };
    struct TriGeom  { Triangle  T; Material mat; };

    std::vector<RectGeom> rects;   // walls + central floor/roof rects
    std::vector<TriGeom>  tris;    // corner floor/roof triangles
    std::vector<Sphere>   spheres; // optional objects
    std::vector<RectLight> lights; // roof area light

    // camera background (not really visible once room is closed)
    Color background(const Ray& r) const {
        double t = 0.5 * (r.dir.y + 1.0);
        return Color((1.0 - t) + t * 0.5, (1.0 - t) + t * 0.7, 1.0);
    }

    enum ObjType { NONE, RECT, TRI, SPHERE };
    struct HitAny { bool hit=false; Hit rec; ObjType type=NONE; int index=-1; };

    HitAny trace_first(const Ray& r, double tmin, double tmax) const {
        Hit temp; HitAny out; double closest = tmax;

        for (int i=0;i<(int)rects.size();++i){
            if (rects[i].R.intersect(r, tmin, closest, temp)) { out={true,temp,RECT,i}; closest=temp.t; }
        }
        for (int i=0;i<(int)tris.size();++i){
            if (tris[i].T.intersect(r, tmin, closest, temp))   { out={true,temp,TRI ,i}; closest=temp.t; }
        }
        for (int i=0;i<(int)spheres.size();++i){
            if (spheres[i].intersect(r, tmin, closest, temp))  { out={true,temp,SPHERE,i}; closest=temp.t; }
        }
        return out;
    }

    bool occluded(const Vec3& p, const Vec3& dir, double maxDist) const {
        Hit h;
        // rects
        for (const auto& g : rects) if (g.R.intersect(Ray(p,dir), 1e-4, maxDist-1e-4, h)) return true;
        // tris
        for (const auto& g : tris)  if (g.T.intersect(Ray(p,dir), 1e-4, maxDist-1e-4, h)) return true;
        // spheres
        for (const auto& s : spheres) if (s.intersect(Ray(p,dir), 1e-4, maxDist-1e-4, h)) return true;
        return false;
    }

    // --- direct MC (same as before) ---
    Color direct_light_mc(const HitAny& h, const Color& albedo, int nSamples, std::mt19937_64& rng) const {
        if (lights.empty() || nSamples<=0) return Color(0,0,0);
        std::uniform_real_distribution<double> U(0.0,1.0);
        const double invPi = 1.0/3.14159265358979323846;
        Color L(0,0,0);
        for (const auto& Lrect : lights){
            double A = Lrect.area();
            int n = std::ceil(std::sqrt((double)nSamples)); // stratify a bit
            int used = 0;
            for (int py=0; py<n && used<nSamples; ++py){
                for (int px=0; px<n && used<nSamples; ++px, ++used){
                    double u = (px + U(rng))/n, v = (py + U(rng))/n;
                    Vec3 y = Lrect.sample(u,v);
                    Vec3 d = y - h.rec.p;
                    double d2 = dot(d,d), d1 = std::sqrt(d2);
                    Vec3 wi = d / d1;
                    double cosx = std::max(0.0, dot(h.rec.n, wi));
                    double cosy = std::max(0.0, dot(Lrect.normal, -wi));
                    if (cosx<=0 || cosy<=0) continue;
                    if (occluded(h.rec.p, wi, d1)) continue;
                    double G = (cosx*cosy)/d2;
                    Color c = Lrect.Le * (A*invPi*G / nSamples);
                    c.r *= albedo.r; c.g *= albedo.g; c.b *= albedo.b;
                    L = L + c;
                }
            }
        }
        return L;
    }

    // cosine hemisphere sampling (as before)
    Vec3 sample_cosine_hemisphere(const Vec3& n, std::mt19937_64& rng) const {
        std::uniform_real_distribution<double> U(0.0, 1.0);
        double r1 = 2.0*3.14159265358979323846*U(rng), r2 = U(rng), r2s = std::sqrt(r2);
        Vec3 local(std::cos(r1)*r2s, std::sin(r1)*r2s, std::sqrt(1.0-r2));
        Vec3 a = (std::fabs(n.x) > 0.1) ? Vec3(0,1,0) : Vec3(1,0,0);
        Vec3 t = normalize(cross(a,n)), b = cross(n,t);
        return normalize(t*local.x + b*local.y + n*local.z);
    }

    // recursive shader (mirror + diffuse GI)
    Color shade_path(const Ray& r, int depth, int directSamples, std::mt19937_64& rng) const {
        if (depth<=0) return Color(0,0,0);
        auto h = trace_first(r, 1e-4, 1e9);
        if (!h.hit) return background(r);

        const Material* m = nullptr;
        if      (h.type==RECT)   m = &rects[h.index].mat;
        else if (h.type==TRI)    m = &tris[h.index].mat;
        else if (h.type==SPHERE) m = &spheres[h.index].mat;
        if (!m) return Color(0,0,0);

        if (m->type == MatType::EMISSIVE) {
        // Only emit if we’re hitting the front face of the lamp
        if (h.rec.front_face)
            return m->emission;
        else
            return Color(0,0,0);
    }

        if (m->type == MatType::MIRROR) {
            Vec3 refl = reflect(r.dir, h.rec.n);
            return shade_path(Ray(h.rec.p, refl), depth-1, directSamples, rng);
        }

        Color Ld = direct_light_mc(h, m->albedo, directSamples, rng);

        double ps = std::min(0.95, std::max({m->albedo.r, m->albedo.g, m->albedo.b}));
        if (depth<=2) ps = 1.0;
        std::uniform_real_distribution<double> U(0.0,1.0);
        if (U(rng) > ps) return Ld;

        Vec3 wi = sample_cosine_hemisphere(h.rec.n, rng);
        Color Li = shade_path(Ray(h.rec.p, wi), depth-1, directSamples, rng);
        Color Lind(m->albedo.r*Li.r/ps, m->albedo.g*Li.g/ps, m->albedo.b*Li.b/ps);
        return Ld + Lind;
    }
};
