#pragma once
#include "color.h"

enum class MatType { LAMBERT, MIRROR, EMISSIVE };

struct Material {
    MatType type = MatType::LAMBERT;
    Color   albedo{0.8,0.8,0.8};   // used by Lambert
    Color   emission{0,0,0};       // used by Emissive (radiance)
    Material() = default;
    Material(MatType t, Color a, Color e = Color{}) : type(t), albedo(a), emission(e) {}
};
