/*! \file symbol_builder.h
 *  \brief Header file for the symbol_builder class and the plcp_header struct.
 *
 * The symbol_builder class is a container for a symbol_builder which is made up of a PHY Header,
 * otherwise known as a PLCP Header, and a payload of bytes. The class also
 * has functions for encoding and decoding the header and payload based on the
 * PHY transmission rates.
 */

#ifndef symbol_builder_H
#define symbol_builder_H

#include <complex>
#include <vector>
#include "rates.h"

#define MAX_FRAME_SIZE 2000

namespace wno
{
    /*!
     * \brief The plcp_header struct is a container for PLCP Headers and their
     *  respective parameters.
     */
    struct plcp_header
    {
        Rate rate;                  //!< The PHY Rate for this frame.
        int length;                 //!< The length of the payload in bytes.
        int num_symbols;            //!< The number of ofdm symbols in the frame.
        unsigned short service;     //!< The service field.

        /*!
         * \brief Constructor for empty plcp_header.
         *
         * -Initializations:
         *  + rate -> RATE_1_2_BPSK i.e. BPSK modulation with 1/2 rate convolutional code
         *  + length -> 0
         *  + num_symbols -> 0
         *  + service -> 0
         */
        plcp_header()
        {
            rate = RATE_1_2_BPSK;
            length = 0;
            num_symbols = 0;
            service = 0;
        }

        /*!
         * \brief Constructor for plcp_header with rate and length known.
         * \param _rate PHY Rate for this frame.
         * \param _length Length of payload in bytes.
         */
        plcp_header(Rate _rate, int _length)
        {
            rate = _rate;
            length = _length;
            service = 0;
        }

        /*!
         * \brief Constructor for plcp_header with rate, length and number of symbols known.
         * \param _rate PHY Rate for this frame.
         * \param _length Length of payload in bytes.
         * \param _num_symbols Number of OFDM symbols in frame.
         */
        plcp_header(Rate _rate, int _length, int _num_symbols)
        {
            rate = _rate;
            length = _length;
            num_symbols = _num_symbols;
            service = 0;
        }

    };

    /*!
     * \brief The symbol_builder class
     *
     * The symbol_builder class is a container for a symbol_builder which is made up of a PHY Header,
     * otherwise known as a PLCP Header, and a payload of bytes. The class also
     * has functions for encoding and decoding the header and payload based on the
     * PHY transmission rates.
     */
    class symbol_builder
    {
    public:

        /****************
         * Constructors *
         ****************/

        /*!
         * \brief Default constructor for empty symbol_builder
         */
        symbol_builder();

        /*!
         * \brief Constructor for symbol_builder with rate and length known.
         * \param rate PHY Rate for this frame.
         * \param length Length of payload in bytes.
         */
        symbol_builder(Rate rate, int length);

        /*!
         * \brief Constructor for symbol_builder with payload and Rate known.
         * \param payload The payload/data/MPDU to be transmitted.
         * \param rate The PHY rate for this frame.
         */
        symbol_builder(std::vector<unsigned char> payload, Rate rate);

        /******************
         * Public Members *
         ******************/

        /*!
         * \brief Public interface for encoding a symbol_builder
         * \return Modulated data as a vector of complex doubles
         */
        std::vector<std::complex<double> > encode();

        /*!
         * \brief Public interface for decoding a plcp_header.
         * \param samples Complex samples representing the encoded header symbol.
         * \return boolean of whether decoding the header was successful or not
         *  based on checking/comparing the 1 bit parity field in the header.
         *  If successful the object's #header field is populated appropriately with
         *  the decoded fields.
         */
        bool decode_header(std::vector<std::complex<double> > samples);

        /*!
         * \brief Public interface for decoding the PHY payload into a symbol_builder.
         * \param samples Complex samples representing the encoded payload symbols.
         * \return boolean of whether decoding the payload was successful or not
         *  based on calculating and comparing the IEEE CRC-32 appended to the end
         *  of the payload. If successful the object's #payload field is populated
         *  with the decoded payload/MPDU.
         */
        bool decode_data(std::vector<std::complex<double> > samples);


        Rate get_rate(){return header.rate;}     //!< Get this symbol_builder's PHY tx rate
        int get_length(){return header.length;}  //!< Get this symbol_builder's payload length
        int get_num_symbols(){return header.num_symbols;} //!< Get the number of OFDM symbols in this symbol_builder
        std::vector<unsigned char> get_payload(){return payload;} //!< Get the payload of this symbol_builder.

    private:

        plcp_header header; //!< This symbol_builder's header parameters
        std::vector<unsigned char> payload; //!< This symbol_builder's payload

        /*!
         * \brief Encodes this symbol_builder's header. The header is always encoded with
         *  BPSK modulation and 1/2 rate convolutional code.
         * \return The modulated header symbol.
         */
        std::vector<std::complex<double> > encoder_header();

        /*!
         * \brief Encodes this symbol_builder's payload. The payload is encoded at the rate
         *  specified in the header.rate field.
         * \return The modulated data.
         */
        std::vector<std::complex<double> > encode_data();

    };

}


#endif // symbol_builder_H
