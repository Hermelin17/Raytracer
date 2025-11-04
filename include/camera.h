#pragma once
#include "ray.h"

// Lecture 4 camera: eye e = (-1,0,0); camera plane is x = 0,
// square [y,z] ∈ [-1,1] × [-1,1]. +x is "forward".
struct Camera {
    Vec3 eye;
    Camera() : eye(-1,0,0) {}

    // u,v in [0,1] from raster -> (y,z) in [-1,1]
    Ray get_ray(double u, double v) const {
        double y = -1.0 + 2.0*u;   // left->right on image becomes y:-1..+1
        double z = -1.0 + 2.0*v;   // bottom->top on image becomes z:-1..+1
        Vec3 pe(0.0, y, z);        // point on camera plane x=0
        return Ray(eye, pe - eye); // direction = pe - eye
    }
};
