/*! \file underlay_decode.cpp
 *  \brief C++ file for the Timing Sync block.
 *
 *  This block is in charge of using the two LTS symbols to align the received frame in time.
 *  It also uses the two LTS symbols to perform an initial frequency offset estimation and
 *  applying the necessary correction.
 */


#include "underlay_decode.h"

#include <algorithm>
#include <cstring>
#include <iostream>

#include "preamble.h"
#include "underlay.h"
#define COEFFTHRESH 0.6
#define CARRYOVER_LENGTH 63
namespace wno
{
    /*!
     * - Initializations:
     */
    underlay_decode::underlay_decode() :
        block("underlay_decode")
    {}


    /*!
     */
    void underlay_decode::work()
    {
        if(input_buffer.size() == 0) return;
        output_buffer.resize(input_buffer.size());
        std::vector<std::complex<double> > input(input_buffer.size() + CARRYOVER_LENGTH);

        memcpy(&input[0],
                &m_carryover[0],
                CARRYOVER_LENGTH*sizeof(std::complex<double>));

        memcpy(&input[CARRYOVER_LENGTH],
                &input_buffer[0],
                input_buffer.size() * sizeof(std::complex<double>));

        // Step through the samples
        for(int x = 0; x < input_buffer.size(); x++)
        {
            output_buffer[x].tag = NONE;
            std::vector<std::complex<double> >::const_iterator first = input.begin() + x;
            std::vector<std::complex<double> >::const_iterator last = input.begin() + x + 63;
            std::vector<std::complex<double> > newVec(first, last);
            if (correlate(newVec))
            {
                output_buffer[x].tag = ULPN;
            }
            else
            {
                //
            }
        }
        memcpy(&output_buffer[0],
                &input[0],
                input_buffer.size() * sizeof(tagged_sample));
        memcpy(&m_carryover[0],
                &input[input_buffer.size()],
                CARRYOVER_LENGTH * sizeof(std::complex<double>));
    }

    /*!
     *  correlates with SPNS and returns 'true' when correlation exceeds the threshold
     */
    bool underlay_decode::correlate(std::vector<std::complex<double> > samples)
    {
        std::complex<double> temp_mul;
        std::complex<double> temp_mean;
        double temp_norm_v;
        double corr_coeff = 0.0;
        double sqr_sum = 0.0;
        temp_mul = (0.0, 0.0);
        temp_mean = (0.0, 0.0);
        sqr_sum = 0.0;

        int N = 64;
        double numr;
        double denm;
        double pn_mean = 0.0;
        for (int y=0; y<N; y++)
        {
            pn_mean += SPNS[y].real();
        }
        pn_mean/=N;
        for (int y=0; y<N; y++)
        {
            temp_mul += samples[y] * SPNS[y];
            sqr_sum += pow(abs(samples[y]),2);
            temp_mean += samples[y];
        }
        temp_mean/=N;
        temp_norm_v = 0.0;
        std::complex<double> scaled_temp_mean = N*pn_mean*temp_mean;
        numr = abs(temp_mul-scaled_temp_mean);
        denm = sqrt(sqr_sum-N*pow(abs(temp_mean),2))*sqrt(N);
        corr_coeff = numr/denm;

        if(corr_coeff>COEFFTHRESH)
        {
            std::cout << "Correlation coefficient above threshold. " << corr_coeff  << std::endl;
            return(true);
        }
        else
            return (false);
    }
}
