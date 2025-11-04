#pragma once
#include "vec3.h"
#include "color.h"

struct RectLight {
    // v0 is one corner on the plane, e1/e2 are edge vectors
    Vec3 v0, e1, e2;        // area = |e1 x e2|
    Vec3 normal;            // unit; points TOWARD the scene
    Color Le{1,1,1};        // emitted radiance (RGB)

    RectLight() = default;
    RectLight(const Vec3& V0, const Vec3& E1, const Vec3& E2, const Vec3& N, const Color& le)
        : v0(V0), e1(E1), e2(E2), normal(normalize(N)), Le(le) {}

    inline double area() const { return length(cross(e1, e2)); }

    // sample a random point uniformly on the rectangle
    inline Vec3 sample(double u, double v) const { return v0 + e1*u + e2*v; }
};
