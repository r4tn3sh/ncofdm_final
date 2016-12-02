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
#include <fstream>
#include <iomanip>

#include "preamble.h"
#include "underlay.h"
#define COEFFTHRESH 0.15
#define CARRYOVER_LENGTH pnSize
namespace wno
{
    /*!
     * - Initializations:
     */
    underlay_decode::underlay_decode() :
        block("underlay_decode"),
        m_carryover(CARRYOVER_LENGTH, 0)
    {}


    /*!
     */
    void underlay_decode::work()
    {
        double corr_coeff = 0.0;
        if(input_buffer.size() == 0) return;
        output_buffer.resize(input_buffer.size());
        std::vector<std::complex<double> > input(input_buffer.size() + CARRYOVER_LENGTH);
        std::ofstream myfile;
        // myfile.open("corr.txt",std::fstream::app);

        memcpy(&input[0],
                &m_carryover[0],
                CARRYOVER_LENGTH*sizeof(std::complex<double>));

        memcpy(&input[CARRYOVER_LENGTH],
                &input_buffer[0],
                input_buffer.size() * sizeof(std::complex<double>));

        // Step through the samples
        // std::cout << "Input buffer size : " << input_buffer.size() << std::endl;
        for(int x = 0; x < input_buffer.size(); x++)
        {
            // output_buffer[x].tag = NONE;
            std::vector<std::complex<double> >::const_iterator first = input.begin() + x;
            std::vector<std::complex<double> >::const_iterator last = input.begin() + x + pnSize-1;
            std::vector<std::complex<double> > newVec(first, last);
            corr_coeff = correlate(newVec);
            // myfile << std::fixed << std::setprecision(8) << corr_coeff << std::endl;
            if(corr_coeff>COEFFTHRESH || corr_coeff<0-COEFFTHRESH)
            {
                std::cout <<  x << " " << corr_coeff << std::endl;
            }
            // if (correlate(newVec))
            // {
            //     // std::cout << "PN seq found at " << x << std::endl;
            //     // output_buffer[x].tag = ULPN;
            // }
            // else
            // {
            //     //
            // }
        }
        // myfile.close();
        memcpy(&output_buffer[0],
                &input[0],
                input_buffer.size() * sizeof(std::complex<double>));
                //input_buffer.size() * sizeof(tagged_sample));
        memcpy(&m_carryover[0],
                &input[input_buffer.size()],
                CARRYOVER_LENGTH * sizeof(std::complex<double>));
    }

    /*!
     *  correlates with SPNS and returns 'true' when correlation exceeds the threshold
     */
    double underlay_decode::correlate(std::vector<std::complex<double> > samples)
    {
        std::complex<double> temp_mul;
        std::complex<double> temp_mean;
        double temp_norm_v;
        double corr_coeff = 0.0;
        double sqr_sum = 0.0;
        temp_mul = (0.0, 0.0);
        temp_mean = (0.0, 0.0);
        sqr_sum = 0.0;

        int N = pnSize;
        std::complex<double> cplx_numr;
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
        cplx_numr = (temp_mul-scaled_temp_mean);
        numr = abs(temp_mul-scaled_temp_mean);
        denm = sqrt(sqr_sum-N*pow(abs(temp_mean),2))*sqrt(N);
        if (denm == 0) // Is it the right way to do it ??
            return(0.00001);
        if (cplx_numr.real()>0)
            corr_coeff = numr/denm;
        else
            corr_coeff = 0.0-numr/denm;
        // corr_coeff = numr/denm;
        // corr_coeff = cplx_numr.real()/denm;
        return(corr_coeff);
    }
}
