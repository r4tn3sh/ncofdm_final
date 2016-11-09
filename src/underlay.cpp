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
        std::vector<std::complex<double> > output;
        for(int x = 0; x < overlay_data.size() / TOTAL_SIZE; x++)
        {
            // Add 240 240 80 80 80 ......
            output[z] = overlay_data[z] + 0.0316227766*0.1*SPNS[x]; // adding signal 15dB below
            z++;
        }
        return output;
    }
}
