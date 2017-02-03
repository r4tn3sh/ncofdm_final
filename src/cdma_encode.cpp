/*! \file cdma_encode.cpp
 *  \brief C++ file for the Timing Sync block.
 *
 *  This block is in charge of using the two LTS symbols to align the received frame in time.
 *  It also uses the two LTS symbols to perform an initial frequency offset estimation and
 *  applying the necessary correction.
 */


#include "cdma_encode.h"

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
namespace wno
{
    /*!
     * - Initializations:
     */
    cdma_encode::cdma_encode(uint32_t pnlength, uint32_t codeidx) :
        block("cdma_encode"),
        m_pnlength(pnlength),
        m_codeidx(codeidx)
    {}


    /*!
     * Receive control information in input buffer.
     * Encode the data using available spreading code.
     * Copy the encoded data to output buffer
     */
    void cdma_encode::work()
    {
        double corr_coeff = 0.0;
        if(input_buffer.size() == 0) return;

        // output buffer size is decided by the spreading code length
        output_buffer.resize(m_pnlength * input_buffer.size());

        std::vector<std::complex<double> > local_chips(m_pnlength);
        std::vector<std::complex<double> > temp_buffer_0(m_pnlength);
        std::vector<std::complex<double> > temp_buffer_1(m_pnlength);

        // Create a small chip sequence from large stored sequence
        // TODO: Better way to do it.
        memcpy(&local_chips[0],
                &chips[m_codeidx],
                m_pnlength * sizeof(std::complex<double>));
        
        // Create two copies of chip sequence
        memcpy(&temp_buffer_0[0],
                &local_chips[0],
                m_pnlength * sizeof(std::complex<double>));
        for(int x = 0; x < input_buffer.size(); x++)
            temp_buffer_1[x] = local_chips[x]*std::complex<double>(-1,0);
        
        // TODO : tag the end and beginning of chip
        // Step through the samples
        // std::cout << "Input buffer size : " << input_buffer.size() << std::endl;
        int in_size = input_buffer.size();
        for(int x = 0; x < in_size; x++)
        {
            if (input_buffer[x].real() > 0) //XXX : assuming bpsk transmission
            {
                memcpy(&output_buffer[x*m_pnlength],
                        &temp_buffer_0[0],
                        m_pnlength * sizeof(std::complex<double>));
            }
            else
            {
                memcpy(&output_buffer[x*m_pnlength],
                        &temp_buffer_1[0],
                        m_pnlength * sizeof(std::complex<double>));
            }
        }
    }
}
