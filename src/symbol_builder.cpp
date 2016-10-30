/*! \file symbol_builder.cpp
 *  \brief C++ file for the symbol_builder class.
 */

#include <arpa/inet.h>
#include <boost/crc.hpp>
#include <iostream>

#include "symbol_builder.h"
#include "parity.h"
#include "viterbi.h"
#include "interleaver.h"
#include "puncturer.h"
#include "modulator.h"

namespace wno
{
    /*!
     * This constructor creates an empty symbol_builder with the default/empty plcp_header constructor
     */
    symbol_builder::symbol_builder()
    {
        payload.reserve(MAX_FRAME_SIZE);
    }

    /*!
     * This constructor creates a complete symbol_builder with header and payload.
     */
    symbol_builder::symbol_builder(std::vector<unsigned char> payload, Rate rate) :
        payload(payload)
    {
        RateParams rate_params = RateParams(rate);
    }

    /*!
     * Public wrapper for encoding the header & payload and concatenating them together into a
     * PHY frame.
     */
    std::vector<std::complex<double> > symbol_builder::encode()
    {
        std::vector<std::complex<double> > payload_samples = encode_data();
        return payload_samples;
    }

    std::vector<std::complex<double> > symbol_builder::encode_data()
    {
        // Get the RateParams
        RateParams rate_params = RateParams(header.rate);

        // Calculate the number of symbols
        int num_symbols = std::ceil(
                double((8 * (payload.size() + 4 /* CRC */) + 6 /* tail */)) /
                double(rate_params.dbps));

        // Calculate the number of data bits/bytes (including padding bits)
        int num_data_bits = num_symbols * rate_params.dbps;
        int num_data_bytes = num_data_bits / 8;

        unsigned short service_field = 0; //FIXME: reason for this variable?

        std::vector<unsigned char> data(num_data_bytes+1, 0);
        memcpy(&data[0], &service_field, 2);
        memcpy(&data[2], payload.data(), payload.size());

        // Calcualate and append the CRC
        boost::crc_32_type crc;
        crc.process_bytes(&data[0], 2 + payload.size());
        unsigned int calculated_crc = crc.checksum();
        memcpy(&data[2 + payload.size()], &calculated_crc, 4);

        // Scramble the data
        std::vector<unsigned char> scrambled(num_data_bytes+1, 0);
        int state = 93, feedback = 0;
        for(int x = 0; x < num_data_bytes; x++)
        {
           feedback = (!!(state & 64)) ^ (!!(state & 8));
           scrambled[x] = feedback ^ data[x];
           state = ((state << 1) & 0x7E) | feedback;
        }
        data.swap(scrambled);

        // Convolutionally encode the data
        std::vector<unsigned char> data_encoded(num_data_bits * 2, 0);
        viterbi v;
        v.conv_encode(&data[0], data_encoded.data(), num_data_bits-6);

        // Puncture the data
        std::vector<unsigned char> data_punctured = puncturer::puncture(data_encoded, header.rate);

        // Interleave the data
        std::vector<unsigned char> data_interleaved = interleaver::interleave(data_punctured);

        // Modulated the data
        std::vector<std::complex<double> > data_modulated = modulator::modulate(data_interleaved, header.rate);

        return data_modulated;
    }



    bool symbol_builder::decode_data(std::vector<std::complex<double> > samples)
    {
        // Get the RateParams
        RateParams rate_params = RateParams(header.rate);

        // Calculate the number of symbols
        int num_symbols = std::ceil(
                double((16 /* service */ + 8 * (header.length + 4 /* CRC */) + 6 /* tail */)) /
                double(rate_params.dbps));

        // Calculate the number of data bits/bytes (including padding bits)
        int num_data_bits = num_symbols * rate_params.dbps;
        int num_data_bytes = num_data_bits / 8;

        // Demodulate the data
        std::vector<unsigned char> demodulated = modulator::demodulate(samples, header.rate);

        // Deinterleave the data
        std::vector<unsigned char> deinterleaved = interleaver::deinterleave(demodulated);

        // Depuncture the data
        std::vector<unsigned char> depunctured = puncturer::depuncture(deinterleaved, header.rate);

        // Convolutionally decode the data
        int data_bits = 16 /* service */ + (header.length + 4 /* CRC */) * 8 + 6 /* tail bits */;
        int data_bytes = data_bits / 8 + 1;
        data_bits = num_data_bits - 6;
        data_bytes = num_data_bytes;
        std::vector<unsigned char> decoded(data_bytes);
        viterbi v;
        v.conv_decode(&depunctured[0], &decoded[0], data_bits);

        // Descramble the data
        std::vector<unsigned char> descrambled(num_data_bytes+1, 0);
        int state = 93, feedback = 0;
        for(int x = 0; x < num_data_bytes; x++)
        {
           feedback = (!!(state & 64)) ^ (!!(state & 8));
           descrambled[x] = feedback ^ decoded[x];
           state = ((state << 1) & 0x7E) | feedback;
        }
        decoded.swap(descrambled);

        // Calculate the CRC
        boost::crc_32_type crc;
        crc.process_bytes(&decoded[0], 2 + header.length);
        unsigned int calculated_crc = crc.checksum();
        unsigned int given_crc = 0;
        memcpy(&given_crc, &decoded[2 + header.length], 4);

        // Verify the CRC
        if(given_crc != calculated_crc)
        {
            std::cerr << "Invalid CRC (length " << header.length << ")" << std::endl;
            // Indicate failure
            return false;
        }
        else
        {
            // Copy the payload
    //        std::vector<unsigned char> payload(length);
            payload.resize(header.length);
            memcpy(&payload[0], &decoded[2 /* skip the service field */], header.length);

            // Fill the output values
    //        data_out.rate = rate;
            memcpy(&header.service, &decoded[0], 2);
    //        data_out.length = length;
            // Indicate success
            return true;
        }

    }

}

