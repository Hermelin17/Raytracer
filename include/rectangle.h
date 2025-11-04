#pragma once
#include "ray.h"
#include "hit.h"

// Axis-free rectangle defined by corner v0 and edges e1, e2.
// Intersection checks 0<=a<=1, 0<=b<=1.
struct Rectangle {
    Vec3 v0, e1, e2;  // area = |e1 x e2|
    Vec3 normal;

    Rectangle() = default;
    Rectangle(const Vec3& V0, const Vec3& E1, const Vec3& E2)
        : v0(V0), e1(E1), e2(E2)
    {
        normal = normalize(cross(e1, e2));
    }

    bool intersect(const Ray& ray, double tmin, double tmax, Hit& rec) const {
        double denom = dot(ray.dir, normal);
        if (std::abs(denom) < 1e-12) return false;
        double t = dot(v0 - ray.origin, normal) / denom;
        if (t < tmin || t > tmax) return false;

        Vec3 p = ray.at(t);
        Vec3 r = p - v0;

        double e1e1 = dot(e1, e1);
        double e2e2 = dot(e2, e2);
        double a = dot(r, e1) / e1e1;
        double b = dot(r, e2) / e2e2;
        if (a < 0.0 || a > 1.0 || b < 0.0 || b > 1.0) return false;

        rec.t = t; rec.p = p; rec.set_face_normal(ray.dir, normal);
        return true;
    }
};
