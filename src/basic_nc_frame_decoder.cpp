/*! \file basic_nc_frame_decoder.cpp
 *  \brief C++ file for the Frame Decoder block.
 *
 * This block works just after FFT operation. It takes vector<64> as
 * input and demaps the data based on subcarrier allocation. It corrects the
 * phase by using pilot subcarriers as reference. It identifies the start
 * of a frame and decodes it.
 */

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <boost/crc.hpp>

#include "basic_nc_frame_decoder.h"
#include "qam.h"
#include "interleaver.h"
#include "underlay.h"
#include "viterbi.h"
#include "parity.h"
#include "rates.h"
#include "modulator.h"
#include "puncturer.h"
#include "interleaver.h"
#include "basic_payload_builder.h"
#include "subcarrier_mapper.h"

namespace wno
{
    /*!
     * - Initializations:
     *   + #m_current_frame -> Reset to a frame of 0 length with RATE_1_2_BPSK
     */
    basic_nc_frame_decoder::basic_nc_frame_decoder(uint64_t sc_map) :
        block("basic_nc_frame_decoder"),
        m_current_frame(FrameData(RateParams(RATE_1_2_BPSK)))
    {
        m_current_frame.Reset(RateParams(RATE_1_2_BPSK), 0, 0);
        m_sc_map = sc_map;
        // map where the data and pilot go
        int loc_count = 0;
        int pilot_count = 0;
        m_total_subcarrier_count = 0;
        for(int x=0; x<64; x++)
        {
            if (((sc_map>>x)&0x1) == 1)
            {
                loc_count++;
                // TODO: check if you are actually using pilots at all
                // XXX: Currently assuming every 8th SC used is pilot
                if(loc_count%8 == 0) // every 8th SC is pilot
                {
                    pilot_count++;
                }
                m_total_subcarrier_count++;
            }
        }
        m_data_subcarrier_count = m_total_subcarrier_count - pilot_count;
    }

    /*!
     * When a start of frame is detected this block first attempts to decode the basic_payload_builder header.
     * If that is successful as determined by a simple parity check on the header bits it
     * then tries to decode the payload of the frame using the parameters it gathered from
     * header.  If that is successful as deteremined by an IEEE CRC-32 check, the decoded payload
     * is passed to the output_buffer to be returned to the receive chain so that it can be passed
     * up to the MAC layer.
     */
    void basic_nc_frame_decoder::work()
    {
        if(input_buffer.size() == 0) return;
        output_buffer.resize(0);

        std::vector<std::complex<double> > demapped(m_data_subcarrier_count);
        std::vector<std::complex<double> > temp_frame(64);
        // Step through each m_total_subcarrier_count sample symbol
        for(int x = 0; x < input_buffer.size(); x++)
        {
            // Look for a start of frame
            if(input_buffer[x].tag == ULPN)
            {

                basic_payload_builder h = basic_payload_builder();

                // Calculate the frame sample count
                int length = 28; // XXX 
                RateParams rate_params = RateParams(RATE_1_2_BPSK);//(RATE_3_4_QAM16);
                // frame_sample_count is actual number of useful samples in the frame
                int frame_sample_count = 528;//h.get_num_symbols() * m_total_subcarrier_count;

                // Start a new frame
                m_current_frame.Reset(rate_params, frame_sample_count, length);
                m_current_frame.samples.resize(frame_sample_count);
                // std::cout << "Decoder : Frame detected -|-|-|-|"<< std::endl;
            }
            // Copy over available symbols
            if(m_current_frame.samples_copied < m_current_frame.sample_count)
            {
                // XXX: CP has already been removed at this point
                // Now samples should be demaped based on subcarrier allocation
                
                // std::cout << "Decoder : Demapping begins " << input_buffer.size() << " "<< x << std::endl;
                // std::cout << "Decoder : Total SCs = " << m_total_subcarrier_count << " " << m_sc_map << std::endl;
                

                // Demap the subcarriers and remove pilots, 64 samples at a time?
                subcarrier_mapper mapper = subcarrier_mapper(m_sc_map);
                memcpy(&temp_frame[0], &input_buffer[x].samples[0], 64 * sizeof(std::complex<double>));
                demapped = mapper.demap(temp_frame); // size of demapped should be m_data_subcarrier_count
                // std::cout << "Decoder : Demapping ends " << demapped.size() << " "<< x << std::endl;

                memcpy(&m_current_frame.samples[m_current_frame.samples_copied], &demapped[0], m_data_subcarrier_count * sizeof(std::complex<double>));
                m_current_frame.samples_copied += m_data_subcarrier_count;
            }

            // Decode the frame if possible
            if(m_current_frame.samples_copied >= m_current_frame.sample_count && m_current_frame.sample_count != 0)
            {
                // std::cout << "Decoder : Decoding begins " << m_current_frame.rate_params.rate<< std::endl;
                basic_payload_builder frame = basic_payload_builder(m_current_frame.rate_params.rate, m_current_frame.length);
                if(frame.decode_data(m_current_frame.samples))
                {
                    output_buffer.push_back(frame.get_payload());
                }
                m_current_frame.sample_count = 0;
            }

            // // Look for a start of frame
            // if(input_buffer[x].tag == ULPN)
            // {

            //     basic_payload_builder h = basic_payload_builder();

            //     // Calculate the frame sample count
            //     int length = 28; // XXX 
            //     RateParams rate_params = RateParams(RATE_1_2_BPSK);//(RATE_3_4_QAM16);
            //     // frame_sample_count is actual number of useful samples in the frame
            //     int frame_sample_count = 528;//h.get_num_symbols() * m_total_subcarrier_count;

            //     // Start a new frame
            //     m_current_frame.Reset(rate_params, frame_sample_count, length);
            //     m_current_frame.samples.resize(frame_sample_count);
            //     std::cout << "Decoder : Frame detected -|-|-|-|"<< std::endl;
            //     continue;
            // }
        }
    }
}


