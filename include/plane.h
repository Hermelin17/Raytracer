#pragma once
#include "ray.h"
#include "hit.h"

struct Plane {
    Vec3 p0;    // any point on plane
    Vec3 n;     // unit normal

    Plane() : p0(0,0,0), n(0,0,1) {}
    Plane(const Vec3& P0, const Vec3& N) : p0(P0), n(normalize(N)) {}

    bool intersect(const Ray& ray, double tmin, double tmax, Hit& rec) const {
        double denom = dot(ray.dir, n);
        if (std::abs(denom) < 1e-9) return false;             // parallel
        double t = dot(p0 - ray.origin, n) / denom;
        if (t < tmin || t > tmax) return false;
        rec.t = t;
        rec.p = ray.at(t);
        rec.set_face_normal(ray.dir, n);                      // ensure outward vs ray
        return true;
    }
};
