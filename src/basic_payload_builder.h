/*! \file basic_payload_builder.h
 *  \brief Header file for the basic_payload_builder class and the plcp_header struct.
 *
 * The basic_payload_builder class is a container for a basic_payload_builder which is made up of a PHY Header,
 * otherwise known as a PLCP Header, and a payload of bytes. The class also
 * has functions for encoding and decoding the header and payload based on the
 * PHY transmission rates.
 */

#ifndef BASIC_PAYLOAD_BUILDER_H
#define BASIC_PAYLOAD_BUILDER_H

#include <complex>
#include <vector>
#include "rates.h"

#define MAX_FRAME_SIZE 2000

namespace wno
{

    /*!
     */
    class basic_payload_builder
    {
    public:

        /****************
         * Constructors *
         ****************/

        /*!
         * \brief Default constructor for empty basic_payload_builder
         */
        basic_payload_builder();

        /*!
         * \brief Constructor for basic_payload_builder with rate and length known.
         * \param rate PHY Rate for this frame.
         * \param length Length of payload in bytes.
         */
        basic_payload_builder(Rate rate, int length);

        /*!
         * \brief Constructor for basic_payload_builder with payload and Rate known.
         * \param payload The payload/data/MPDU to be transmitted.
         * \param rate The PHY rate for this frame.
         */
        basic_payload_builder(std::vector<unsigned char> payload, Rate rate);

        /******************
         * Public Members *
         ******************/

        /*!
         * \brief Public interface for encoding a basic_payload_builder
         * \return Modulated data as a vector of complex doubles
         */
        std::vector<std::complex<double> > encode();

        /*!
         * \brief Public interface for decoding the PHY payload into a BASIC_PAYLOAD_BUILDER.
         * \param samples Complex samples representing the encoded payload symbols.
         * \return boolean of whether decoding the payload was successful or not
         *  based on calculating and comparing the IEEE CRC-32 appended to the end
         *  of the payload. If successful the object's #payload field is populated
         *  with the decoded payload/MPDU.
         */
        bool decode_data(std::vector<std::complex<double> > samples);


        Rate get_rate(){return m_rate;}     //!< Get this BASIC_PAYLOAD_BUILDER's PHY tx rate
        int get_length(){return m_length;}  //!< Get this BASIC_PAYLOAD_BUILDER's payload length
        int get_num_symbols(){return num_symbols;} //!< Get the number of OFDM symbols in this BASIC_PAYLOAD_BUILDER
        std::vector<unsigned char> get_payload(){return payload;} //!< Get the payload of this BASIC_PAYLOAD_BUILDER.

    private:

        Rate m_rate;
        int m_length;
        int num_symbols;
        std::vector<unsigned char> payload; //!< This BASIC_PAYLOAD_BUILDER's payload

        /*!
         * \brief Encodes this BASIC_PAYLOAD_BUILDER's payload. The payload is encoded at the rate
         *  specified in the header.rate field.
         * \return The modulated data.
         */
        std::vector<std::complex<double> > encode_data();

    };

}


#endif // BASIC_PAYLOAD_BUILDER_H
