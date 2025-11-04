#include <fstream>
#include "vec3.h"
#include "color.h"

int main(){
    const int W = 512, H = 256;
    std::ofstream out("image.ppm", std::ios::binary);
    out << "P6\n" << W << " " << H << "\n255\n";

    // simple test gradient: red→green with a blue stripe
    double maxVal = 1.0; // keep 1.0 so we don’t need normalization yet
    for(int j=H-1;j>=0;--j){
        for(int i=0;i<W;++i){
            double u = double(i) / (W-1);
            double v = double(j) / (H-1);
            Color c( u, v, 0.25 + 0.75*std::sin(6.28318*u) * 0.5 + 0.5 );
            uint8_t R,G,B; to_u8(c, R,G,B, 1.0/maxVal);
            out.put(char(R)).put(char(G)).put(char(B));
        }
    }
    return 0;
}
