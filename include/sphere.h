#pragma once
#include "ray.h"
#include "hit.h"
#include "material.h"

struct Sphere {
    Vec3 c; double r;
    Material mat;

    Sphere() : c(0,0,0), r(1) {}
    Sphere(const Vec3& C, double R, const Material& M) : c(C), r(R), mat(M) {}

    // Return true if hit in [tmin, tmax]; fill out 'rec'
    bool intersect(const Ray& ray, double tmin, double tmax, Hit& rec) const {
        // Solve |o + td - c|^2 = r^2
        Vec3 oc = ray.origin - c;
        double a = dot(ray.dir, ray.dir);
        double half_b = dot(oc, ray.dir);            // = b/2
        double cterm = dot(oc, oc) - r*r;

        double disc = half_b*half_b - a*cterm;
        if (disc < 0.0) return false;
        double sqrtd = std::sqrt(disc);

        // Find nearest root in range
        double root = (-half_b - sqrtd) / a;
        if (root < tmin || root > tmax) {
            root = (-half_b + sqrtd) / a;
            if (root < tmin || root > tmax) return false;
        }

        rec.t = root;
        rec.p = ray.at(rec.t);
        Vec3 outward = (rec.p - c) / r;
        rec.set_face_normal(ray.dir, outward);
        return true;
    }
};
