#pragma once
#include "vec3.h"
#include "color.h"

struct Ray {
    Vec3 origin;
    Vec3 dir;

    Ray() = default;
    Ray(const Vec3& o, const Vec3& d) : origin(o), dir(normalize(d)) {}

    Vec3 at(double t) const { return origin + dir * t; }
};
