/*! \file underlay_tagged_decode.cpp
 *  \brief C++ file for the Timing Sync block.
 *
 *  This block is in charge of using the two LTS symbols to align the received frame in time.
 *  It also uses the two LTS symbols to perform an initial frequency offset estimation and
 *  applying the necessary correction.
 */


#include "underlay_tagged_decode.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "preamble.h"
#include "underlay.h"
#define COEFFTHRESH 0.1
#define UPCOEFFTHRESH 0.15
#define SEARCHWINDOW 10
#define CARRYOVER_LENGTH pnSize
namespace wno
{
    /*!
     * - Initializations:
     */
    underlay_tagged_decode::underlay_tagged_decode() :
        block("underlay_tagged_decode"),
        m_carryover(CARRYOVER_LENGTH, 0)
    {}


    /*!
     */
    void underlay_tagged_decode::work()
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
        int in_size = input_buffer.size();
        int next_x = 0;
        int conf = prev_conf;
        for(int x = 0; x < input_buffer.size(); x++)
        {
            // Pass through the sample
            output_buffer[x].sample = input_buffer[x];
            // Initialize the tag
            output_buffer[x].tag = NONE;
            std::vector<std::complex<double> >::const_iterator first = input.begin() + x;
            std::vector<std::complex<double> >::const_iterator last = input.begin() + x + pnSize-1;
            std::vector<std::complex<double> > newVec(first, last);
            if (x==next_x)
            {
                conf--;
                // std::cout<< conf  << "*"<< x<< "*"<< next_x << std::endl;
                corr_coeff = correlate(newVec);
                next_x = (x+1)%in_size;
            }
            else
            {
                continue;
            }
            if(corr_coeff>COEFFTHRESH || corr_coeff<0-COEFFTHRESH)
            {
                //XXX: Tag the output *********
                output_buffer[x].tag = ULPN;

                if (corr_coeff>0) // bit '1' received
                {
                    if (prev_bit == 1) bits_in_error++;
                    prev_bit = 1;
                }
                else if (corr_coeff<0) // bit '0' received
                {
                    if (prev_bit == 0) bits_in_error++;
                    prev_bit = 0;
                }

                if(corr_coeff>UPCOEFFTHRESH || corr_coeff< 0 - UPCOEFFTHRESH) // very high correlation received
                {
                    conf = 100; // increase confidance to max
                }
                else 
                {
                    if (conf > 0) conf = 100; // peak detected in expected zone
                }

                if (conf < 100)
                    next_x = (x+1)%in_size;
                else
                    next_x = (x+pnSize-SEARCHWINDOW)%in_size; // go to 50 samples behind expected peak

                // std::cout << "UL tagged decode " <<  x << " " << corr_coeff << " " << bits_in_error  << " " << next_x<< " " << prev_bit << std::endl;
            }

        }
        prev_conf = conf;
        memcpy(&m_carryover[0],
                &input[input_buffer.size()],
                CARRYOVER_LENGTH * sizeof(std::complex<double>));
    }

    /*!
     *  correlates with SPNS and returns 'true' when correlation exceeds the threshold
     */
    double underlay_tagged_decode::correlate(std::vector<std::complex<double> > samples)
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
