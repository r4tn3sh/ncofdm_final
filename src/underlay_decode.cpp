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

namespace wno
{
    /*!
     * - Initializations:
     */
    underlay_decode::underlay_decode() :
        block("underlay_decode")
    {}

    int lts_count = 0;

    /*!
     */
    void underlay_decode::work()
    {
        if(input_buffer.size() == 0) return;
        output_buffer.resize(input_buffer.size());
        // Step through the samples
        for(int x = 0; x < input_buffer.size(); x++)
        {
            output_buffer[x].tag = NONE;
        }
    }

    void underlay_decode::correlate(std::complex<double> rxdata)
    {
    }

}
