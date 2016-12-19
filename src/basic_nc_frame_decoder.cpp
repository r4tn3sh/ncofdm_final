/*! \file frame_decoder.cpp
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

#include "frame_decoder.h"
#include "qam.h"
#include "interleaver.h"
#include "viterbi.h"
#include "parity.h"
#include "rates.h"
#include "modulator.h"
#include "puncturer.h"
#include "interleaver.h"
#include "basic_payload_builder.h"

namespace wno
{
    /*!
     * - Initializations:
     *   + #m_current_frame -> Reset to a frame of 0 length with RATE_1_2_BPSK
     */
    frame_decoder::frame_decoder(uint64_t sc_map) :
        block("frame_decoder"),
        m_current_frame(FrameData(RateParams(RATE_1_2_BPSK)))
    {
        m_current_frame.Reset(RateParams(RATE_1_2_BPSK), 0, 0);
        m_sc_map = sc_map;
        // count number of bits in the allocation vector
        unsigned int temp_sc_count;
        temp_sc_count = m_sc_map - ((m_sc_map >> 1) & 033333333333) - ((m_sc_map >> 2) & 011111111111);
        m_total_subcarrier_count = ((temp_sc_count + (temp_sc_count >> 3)) & 030707070707) % 63;
    }

    /*!
     * When a start of frame is detected this block first attempts to decode the basic_payload_builder header.
     * If that is successful as determined by a simple parity check on the header bits it
     * then tries to decode the payload of the frame using the parameters it gathered from
     * header.  If that is successful as deteremined by an IEEE CRC-32 check, the decoded payload
     * is passed to the output_buffer to be returned to the receive chain so that it can be passed
     * up to the MAC layer.
     */
    void frame_decoder::work()
    {
        if(input_buffer.size() == 0) return;
        output_buffer.resize(0);

        // Step through each m_total_subcarrier_count sample symbol
        for(int x = 0; x < input_buffer.size(); x++)
        {
            // Copy over available symbols
            if(m_current_frame.samples_copied < m_current_frame.sample_count)
            {
                memcpy(&m_current_frame.samples[m_current_frame.samples_copied], &input_buffer[x].samples[0], m_total_subcarrier_count * sizeof(std::complex<double>));
                m_current_frame.samples_copied += m_total_subcarrier_count;
            }

            // Decode the frame if possible
            if(m_current_frame.samples_copied >= m_current_frame.sample_count && m_current_frame.sample_count != 0)
            {
                basic_payload_builder frame = basic_payload_builder(m_current_frame.rate_params.rate, m_current_frame.length);
                if(frame.decode_data(m_current_frame.samples))
                {
                    output_buffer.push_back(frame.get_payload());
                }
                m_current_frame.sample_count = 0;
            }

            // Look for a start of frame
            if(input_buffer[x].tag == ULPN)
            {
                // XXX: CP has already been removed at this point
                // Now samples should be demaped based on subcarrier allocation
                //
                // Demap the subcarriers and remove pilots, 64 samples at a time
                subcarrier_mapper mapper = subcarrier_mapper(m_sc_map);
                std::vector<std::complex<double> > demapped = mapper.demap(samples);

                basic_payload_builder h = basic_payload_builder();

                // Calculate the frame sample count
                int length = pnSize; //XXX: assume frame length to be same as pnSize
                RateParams rate_params = RATE_1_2_BPSK;
                int frame_sample_count = h.get_num_symbols() * m_total_subcarrier_count;

                // Start a new frame
                m_current_frame.Reset(rate_params, frame_sample_count, length);
                m_current_frame.samples.resize(h.get_num_symbols() * m_total_subcarrier_count);
                continue;
            }
        }
    }
}


