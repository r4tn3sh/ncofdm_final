/*! \file frame_builder.cpp
 *  \brief C++ for the Frame Builder class.
 *
 * This class takes input data (MPDUs) and builds PHY layer frames by first prepending a PHY header
 * and appending an IEEE CRC-32 checksum to the data. It then encodes the data by scrambling,
 * convolutional coding the data at the coding rate specified (this may or may not require puncturing),
 * and interleaving the data. It then modulates the data as specified. The modulated data
 * is then mapped into symbols with the data being mapped to the respective data subcarriers with pilots
 * and nulls being mapped to their respective subcarriers. Each symbol is then run through an IFFT to
 * convert it to time domain an a cyclic prefix is attached to each symbol. Finally, the symbols are
 * concatenated together and a preamble is prepended to complete the frame. The frame is then returned
 * so that it can be passed to the USRP for transmission.
 */

#include <arpa/inet.h>
#include <boost/crc.hpp>
#include <iostream>
#include <fstream>
#include <stddef.h>

#include "frame_builder.h"
#include "interleaver.h"
#include "qam.h"
#include "ppdu.h"
#include "symbol_mapper.h"
#include "fft.h"
#include "preamble.h"
#include "rates.h"
#include "viterbi.h"
#include "parity.h"
#include "modulator.h"
#include "puncturer.h"
#include "underlay.h"


#define preamble_length 256 //!< Not cyclic prefixed at this point yet

namespace wno
{
    /*!
    * -Initializations
    *  + #m_ifft -> 64 point IFFT object
    */
    frame_builder::frame_builder() :
        m_ifft(64)
    {
    }

    /*!
     * The build_frame function is the main function where the input data is converted to the raw
     * output samples. The ppdu class is used to append the PHY header, scramble, convolutional code,
     * interleaves, and modulates the input data.  The symbol mapper converts the modulated data into
     * symbols and maps the data, pilots, and nulls to their respective subcarriers. The IFFT performs
     * and IFFT (go figure). The cyclic prefixes are added and finally the preamble is prepended to
     * complete the frame which is then returned to be passed to the usrp block.
     */
    std::vector<std::complex<double> > frame_builder::build_frame(std::vector<unsigned char> payload, Rate rate)
    {
        //Append header, scramble, code, interleave, & modulate
        ppdu ppdu_frame(payload, rate);        
        std::vector<std::complex<double> > samples = ppdu_frame.encode();
        std::cout << "Encode frame :" << payload.size() << " --> " << samples.size() << std::endl;

        // Map the subcarriers and insert pilots
        symbol_mapper mapper = symbol_mapper();
        std::vector<std::complex<double> > mapped = mapper.map(samples);
        std::cout << "Map subcarrier :" << samples.size() << " --> " << mapped.size() << std::endl;

        // Perform the IFFT
        m_ifft.inverse(mapped);

        // Add the cyclic prefixes
        std::vector<std::complex<double> > prefixed(mapped.size() * 80 / 64);
        for(int x = 0; x < mapped.size() / 64; x++)
        {
            memcpy(&prefixed[x*80], &mapped[x*64+48], 16*sizeof(std::complex<double>));
            memcpy(&prefixed[x*80+16], &mapped[x*64], 64*sizeof(std::complex<double>));
        }
        std::cout << "Add CP :" << mapped.size() << " --> " << prefixed.size() << std::endl;

        // ---------- Underlay after preamble ----------
        // Prepend the preamble
        std::vector<std::complex<double> > frame(prefixed.size() + 320);

        memcpy(&frame[0], &PREAMBLE_SAMPLES[0], 320 * sizeof(std::complex<double>));
        memcpy(&frame[320], &prefixed[0], prefixed.size() * sizeof(std::complex<double>));
        std::cout << "Add Preamble :" << prefixed.size() << " --> " << frame.size() << std::endl;

        //XXX: Make the payload size to a multiple of PN-sequence size. (for convenience)
        int pad_length = pnSize - frame.size()%pnSize;
        std::cout << frame.size() << "-*- padding zeros : " << pad_length <<  std::endl;
        std::vector<std::complex<double> > paddedframe(frame.size() + pad_length);
        memcpy(&paddedframe[0], &frame[0], frame.size() * sizeof(std::complex<double>));
        for(int x=0; x<pad_length; x++)
        {
            paddedframe[frame.size()+x] = std::complex<double>(0.0,0.0);
        }
        // save data for analysis
        std::ofstream outfile;
        // if (not null)
        outfile.open("overlay_data.dat", std::ofstream::binary);
        if (outfile.is_open())
            outfile.write((const char*)&paddedframe.front(), paddedframe.size()*sizeof(std::complex<double>));

        outfile.close();
        //XXX:Adding underlay after preamble. still in testing phase
        underlay ul = underlay();
        std::vector<std::complex<double> > combined = ul.add_underlay(paddedframe);
        
        // Return the samples
        return combined;
        // ---------------------------------------------
    }
}

