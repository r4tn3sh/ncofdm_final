#include "underlay.h"

#define TOTAL_SIZE 240
namespace wno
{
    underlay::underlay()
    {
    }
    std::vector<std::complex<double> > underlay::add_underlay(std::vector<std::complex<double> > overlay_data)
    {
        // Assuming that average amplitude of overlay_data is ~0.1
        int z = 0;
        std::vector<std::complex<double> > output = overlay_data;
        for(int x = 0; x < overlay_data.size(); x++)
        {
            // Add 240 240 80 80 80 ......
            output[x] += std::complex<double>(0.00316227766,0)*SPNS[z]; // adding signal 15dB below
            z++;
            if(z>63)
                z=0;
        }
        return output;
    }
    
    std::vector<std::complex<double> > underlay::decode_underlay(std::vector<std::complex<double> > rx_overlay_data)
    {
        std::vector<std::complex<double> > output = rx_overlay_data;
        return output;
    }
}
