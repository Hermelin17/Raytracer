#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

struct Color {
    double r=0, g=0, b=0;       // 0..inf (we’ll normalize later)
    Color() = default;
    Color(double R,double G,double B):r(R),g(G),b(B){}
    Color operator+(const Color& o) const { return {r+o.r,g+o.g,b+o.b}; }
    Color operator*(double s) const { return {r*s,g*s,b*s}; }
};

// gamma 2.2 and clamp to 0..255
inline void to_u8(const Color& c, uint8_t& R, uint8_t& G, uint8_t& B, double invMax=1.0){
    auto tonemap = [&](double x){
        x = std::max(0.0, x*invMax);
        x = std::pow(x, 1.0/2.2);
        x = std::min(1.0, x);
        return static_cast<uint8_t>(std::round(255.0*x));
    };
    R = tonemap(c.r); G = tonemap(c.g); B = tonemap(c.b);
}
