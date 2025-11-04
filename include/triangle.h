#pragma once
#include "ray.h"
#include "hit.h"

struct Triangle {
    Vec3 v0, v1, v2;
    Vec3 normal;

    Triangle() = default;
    Triangle(const Vec3& A, const Vec3& B, const Vec3& C)
        : v0(A), v1(B), v2(C)
    {
        normal = normalize(cross(v1 - v0, v2 - v0));
    }

    bool intersect(const Ray& ray, double tmin, double tmax, Hit& rec) const {
        const double EPS = 1e-12;
        Vec3 E1 = v1 - v0;
        Vec3 E2 = v2 - v0;
        Vec3 P = cross(ray.dir, E2);
        double det = dot(E1, P);
        if (std::abs(det) < EPS) return false;
        double invDet = 1.0 / det;

        Vec3 T = ray.origin - v0;
        double u = dot(T, P) * invDet;
        if (u < 0.0 || u > 1.0) return false;

        Vec3 Q = cross(T, E1);
        double v = dot(ray.dir, Q) * invDet;
        if (v < 0.0 || u + v > 1.0) return false;

        double t = dot(E2, Q) * invDet;
        if (t < tmin || t > tmax) return false;

        rec.t = t; rec.p = ray.at(t); rec.set_face_normal(ray.dir, normal);
        return true;
    }
};
