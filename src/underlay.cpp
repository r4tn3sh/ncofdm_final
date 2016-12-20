#include "underlay.h"
#include <numeric>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>

#define TOTAL_SIZE 240
// #define UL_AMP 0.0125 // 20dB below when all 64 SCs are used
// #define NO_AMP 0.004 // actually std dev for -30dB
#define UL_AMP 0.022 // 15dB below when all 64 SCs are used
#define NO_AMP 0.0//22 // 15dB
namespace wno
{
    underlay::underlay()
    {
        polarity = 1;
    }
    //XXX: overlay_data is assumed to be same as frame size (= pnSize)
    std::vector<std::complex<double> > underlay::add_underlay(std::vector<std::complex<double> > overlay_data)
    {
        std::vector<double>abs_data(overlay_data.size());// = std::abs(overlay_data);
        std::cout << "Buffer of size "<< overlay_data.size() << std::endl;
        std::srand(std::time(0)); // use current time as seed for random generator

        for(int x = 0; x < overlay_data.size(); x++)
        {
            abs_data[x] = pow(std::abs(overlay_data[x]),2);
        }
        double sum = std::accumulate(abs_data.begin(), abs_data.end(), 0.0);
        double avg_power = sum/overlay_data.size();
        std::cout << "Avg power of the data is " << avg_power << std::endl;

        // Assuming that average amplitude of overlay_data is ~0.1
        int z = 0;
        // int polarity = 1; // transmit alternate +1 and -1
        double noise;
        std::default_random_engine generator;
        std::normal_distribution<double> distribution(0.0,NO_AMP);
        std::vector<std::complex<double> > output = overlay_data;
        for(int x = 0; x < overlay_data.size(); x++)
        {
            abs_data[x] = std::abs(overlay_data[x]);

            // Add underlay signal
            output[x] += std::complex<double>(polarity*UL_AMP,0)*SPNS[z]; // adding signal 12dB below

            // XXX: Add noise for simulation
            noise = distribution(generator);
            output[x] += std::complex<double>(noise,0); // Only for simulation 
            z++;
            if(z>pnSize-1)
            {
                z=0;
                polarity  = 0-polarity;
            }
        }
        for(int x = 0; x < overlay_data.size(); x++)
        {
            abs_data[x] = pow(std::abs(output[x]),2);
        }
        sum = std::accumulate(abs_data.begin(), abs_data.end(), 0.0);
        avg_power = sum/overlay_data.size();
        std::cout << "Avg power of the output is " << avg_power << std::endl;
        return output;
    }

    std::vector<std::complex<double> > underlay::decode_underlay(std::vector<std::complex<double> > rx_overlay_data)
    {
        std::vector<std::complex<double> > output = rx_overlay_data;
        return output;
    }
}
