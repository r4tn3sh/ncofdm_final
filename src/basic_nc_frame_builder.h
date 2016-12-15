/*! \file basic_nc_frame_builder.h
 *  \brief Header file for the Frame Builder class.
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


#ifndef BASIC_NC_FRAME_BUILDER_H
#define BASIC_NC_FRAME_BUILDER_H

#include <vector>
#include <complex>

#include "fft.h"
#include "rates.h"

namespace wno
{
    /*!
     * \brief The basic_nc_frame_builder class.
     *
     *  Inputs vector of unsigned chars representing the payload (MPDU) and the transmission PHY
     *  transmission rate.
     *
     *  Outputs vector of raw complex samples representing the digital base-band time domain signal
     *  to be passed to the usrp class for up-conversion and transmission over the air.
     *
     *  This class takes input data (MPDUs) and builds PHY layer frames by first prepending a PHY header
     *  and appending an IEEE CRC-32 checksum to the data. It then encodes the data by scrambling,
     *  convolutional coding the data at the coding rate specified (this may or may not require puncturing),
     *  and interleaving the data. It then modulates the data as specified. The modulated data
     *  is then mapped into symbols with the data being mapped to the respective data subcarriers with pilots
     *  and nulls being mapped to their respective subcarriers. Each symbol is then run through an IFFT to
     *  convert it to time domain an a cyclic prefix is attached to each symbol. Finally, the symbols are
     *  concatenated together and a preamble is prepended to complete the frame. The frame is then returned
     *  so that it can be passed to the USRP for transmission.
     */
    class basic_nc_frame_builder
    {
    public:

        /*!
         * \brief Constructor for basic_nc_frame_builder class
         */
        basic_nc_frame_builder();

        /*!
         * \brief Main function for building a PHY frame
         * \param payload (MPDU) the data that needs to be transmitted over the air.
         * \param rate the PHY transmission rate at which to transmit the respective data at.
         * \return A vector of complex doubles representing the digital base-band time domain signal
         *  to be passed to the usrp class for up-conversion and transmission over the air.
         */
        std::vector<std::complex<double> >  build_frame(std::vector<unsigned char> payload, Rate rate, uint64_t sc_map);

    private:

        fft m_ifft; //!< The fft instance used to perform the inverse FFT on the OFDM symbols

    };
}


#endif // BASIC_NC_FRAME_BUILDER_H
