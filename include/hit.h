#pragma once
#include "vec3.h"

struct Hit {
    double t = 0.0;     // ray parameter at intersection
    Vec3 p;             // hit point
    Vec3 n;             // surface normal (unit)
    bool front_face = true;

    // Ensure normal always points *against* the incoming ray
    inline void set_face_normal(const Vec3& ray_dir, const Vec3& outward_normal){
        front_face = dot(ray_dir, outward_normal) < 0.0;
        n = front_face ? outward_normal : outward_normal * -1.0;
    }
};
