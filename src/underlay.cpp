#include "underlay.h"
#include <numeric>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>

#define TOTAL_SIZE 240
#define UL_AMP 0.0159
namespace wno
{
    underlay::underlay()
    {
    }
    std::vector<std::complex<double> > underlay::add_underlay(std::vector<std::complex<double> > overlay_data)
    {
        std::vector<double>abs_data(overlay_data.size());// = std::abs(overlay_data);
        std::cout << "Buffer of size "<< overlay_data.size() << std::endl;
        std::srand(std::time(0)); // use current time as seed for random generator
        // for(int x = 0; x < overlay_data.size(); x++)
        // {
        //     abs_data[x] = std::abs(overlay_data[x]);
        //     std::cout << "Abs  " << abs_data[x] << std::endl;
        // }
        // double sum = std::accumulate(abs_data.begin(), abs_data.end(), 0.0);
        // std::cout << "Sum of data in buffer of size "<< overlay_data.size()<< " is " << sum << std::endl;
        // Assuming that average amplitude of overlay_data is ~0.1
        int z = 0;
        int polarity = 1; // transmit alternate +1 and -1
        double noise;
        std::default_random_engine generator;
        std::normal_distribution<double> distribution(0.1*UL_AMP,0.1*UL_AMP);
        std::vector<std::complex<double> > output = overlay_data;
        for(int x = 0; x < overlay_data.size(); x++)
        {
            abs_data[x] = std::abs(overlay_data[x]);
            // Add 240 240 80 80 80 ......
            // output[x] += std::complex<double>(0.00316227766,0)*SPNS[z]; // adding signal 15dB below
            // output[x] += std::complex<double>(0.1*abs_data[x],0)*SPNS[z]; // adding signal 15dB below
            // output[x] += std::complex<double>(polarity*0.019342,0)*SPNS[z]; // adding signal 12dB below
            output[x] += std::complex<double>(polarity*UL_AMP,0)*SPNS[z]; // adding signal 12dB below
            noise = distribution(generator);
            std::cout << noise << std::endl;
            output[x] += std::complex<double>(noise,0); // adding signal 12dB below
            z++;
            if(z>pnSize-1)
            {
                z=0;
                polarity  = 0-polarity;
            }
        }
        return output;
    }
    
    std::vector<std::complex<double> > underlay::decode_underlay(std::vector<std::complex<double> > rx_overlay_data)
    {
        std::vector<std::complex<double> > output = rx_overlay_data;
        return output;
    }
}
