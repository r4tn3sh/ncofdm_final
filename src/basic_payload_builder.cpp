/*! \file basic_payload_builder.cpp
 *  \brief C++ file for the basic_payload_builder class.
 *
 */

#include <arpa/inet.h>
#include <boost/crc.hpp>
#include <iostream>

#include "basic_payload_builder.h"
#include "parity.h"
#include "viterbi.h"
#include "interleaver.h"
#include "puncturer.h"
#include "modulator.h"

namespace wno
{
    /*!
     * This constructor creates an empty BASIC_PAYLOAD_BUILDER with the default/empty  constructor
     */
    basic_payload_builder::basic_payload_builder()
    {
        payload.reserve(MAX_FRAME_SIZE);
        // m_rate and m_length don't have default value
    }

    /*!
     * This constructor creates a BASIC_PAYLOAD_BUILDER with a , but no payload field.
     * Useful for the receiver
     */
    basic_payload_builder::basic_payload_builder(Rate rate, int length)
    {
        RateParams rate_params = RateParams(rate);
        m_rate = rate;
        m_length = length;
        num_symbols = std::ceil(
                double((8 * (m_length + 4 /* CRC */) + 6 /* tail */)) /
                double(rate_params.dbps));
        payload.reserve(MAX_FRAME_SIZE);
        // std::cout << "Payload construct : rate = "<< m_rate << ", length = "<<m_length<<std::endl;
    }


    /*!
     * This constructor creates a complete BASIC_PAYLOAD_BUILDER with   payload.
     * Useful for the transmitter
     */
    basic_payload_builder::basic_payload_builder(std::vector<unsigned char> payload, Rate rate) :
        payload(payload)
    {
        RateParams rate_params = RateParams(rate);
        m_rate = rate;
        m_length = payload.size();
        num_symbols = std::ceil(
                double((8 * (m_length + 4 /* CRC */) + 6 /* tail */)) /
                double(rate_params.dbps));

        std::cout << "Payload construct : rate = "<< m_rate << ", length = "<<m_length<<std::endl;
    }

    /*!
     * Public wrapper for encoding the  payload and concatenating them together into a
     * PHY frame.
     */
    std::vector<std::complex<double> > basic_payload_builder::encode()
    {
        std::vector<std::complex<double> > payload_samples = encode_data();
        std::vector<std::complex<double> > basic_payload_builder_samples = std::vector<std::complex<double> >(payload_samples.size());
        memcpy(&basic_payload_builder_samples[0], payload_samples.data(), payload_samples.size() * sizeof(std::complex<double>));
        return basic_payload_builder_samples;
    }

    std::vector<std::complex<double> > basic_payload_builder::encode_data()
    {
        // Get the RateParams
        RateParams rate_params = RateParams(m_rate);

        // Calculate the number of symbols
        num_symbols = std::ceil(
                double((8 * (payload.size() + 4 /* CRC */) + 6 /* tail */)) /
                double(rate_params.dbps));

        // Calculate the number of data bits/bytes (including padding bits)
        int num_data_bits = num_symbols * rate_params.dbps;
        int num_data_bytes = num_data_bits / 8;
        std::cout << "Payload encode : num_symbols = "<< num_symbols << ", length = "<<m_length<< ", Bits = "<< num_data_bits << ", Bytes = " << num_data_bytes << std::endl;

        // Concatenate the service and payload
        // XXX: there is no service anymore
        std::vector<unsigned char> data(num_data_bytes, 0);
        memcpy(&data[0], payload.data(), payload.size());

        // Calcualate and append the CRC
        boost::crc_32_type crc;
        crc.process_bytes(&data[0], payload.size());
        unsigned int calculated_crc = crc.checksum();
        memcpy(&data[payload.size()], &calculated_crc, 4);
        std::cout << "Payload : crc added  "<< payload.size() << " --> "<< data.size() <<std::endl;

        // Scramble the data
        // std::vector<unsigned char> scrambled(num_data_bytes+1, 0);
        // int state = 93, feedback = 0;
        // for(int x = 0; x < num_data_bytes; x++)
        // {
        //    feedback = (!!(state & 64)) ^ (!!(state & 8));
        //    scrambled[x] = feedback ^ data[x];
        //    state = ((state << 1) & 0x7E) | feedback;
        // }
        // data.swap(scrambled);

        // Convolutionally encode the data
        std::vector<unsigned char> data_encoded(num_data_bits * 2, 0);
        viterbi v;
        v.conv_encode(&data[0], data_encoded.data(), num_data_bits-6);
        std::cout << "Payload : conv encoded  "<< data.size() << " --> "<< data_encoded.size() <<std::endl;

        // Modulated the data
        std::vector<std::complex<double> > data_modulated = modulator::modulate(data_encoded, m_rate);

        // for (int x=0; x<data_modulated.size(); x++)
        //     std::cout<< data_modulated[x] << " ";
        // std::cout <<std::endl;

        // TODO: Add a header here for each frame with some information, make size to be 64
        // should this header have avg power same as the modulated data??

        return data_modulated;
    }


    bool basic_payload_builder::decode_data(std::vector<std::complex<double> > samples)
    {
        // std::cout << "Payload decode : rate = "<< m_rate << ", length = "<<m_length<<std::endl;
        // Get the RateParams
        RateParams rate_params = RateParams(m_rate);

        // Calculate the number of data bits/bytes (including padding bits)
        int num_data_bits = num_symbols * rate_params.dbps;
        int num_data_bytes = num_data_bits / 8;

        // for (int x=0; x<samples.size(); x++)
        //     std::cout<<samples[x] << " ";
        // std::cout <<std::endl;

        // Demodulate the data
        std::vector<unsigned char> demodulated = modulator::demodulate(samples, m_rate);
        // TODO Delete this
        // for (int y=0; y<demodulated.size(); y++)
        // {
        //     std::cout << (int)demodulated[y] << "; ";
        // }
        // std::cout << std::endl;
        // std::cout << std::endl;

        // Convolutionally decode the data
        int data_bits = (m_length + 4 /* CRC */) * 8 + 6 /* tail bits */;
        int data_bytes = data_bits / 8 + 1;
        data_bits = num_data_bits - 6;
        data_bytes = num_data_bytes;
        std::vector<unsigned char> decoded(data_bytes);
        viterbi v;
        // v.conv_decode(&depunctured[0], &decoded[0], data_bits);
        v.conv_decode(&demodulated[0], &decoded[0], data_bits);
        // std::cout << "Payload : viterbi decoded  "<< demodulated.size() << " --> "<< decoded.size() <<std::endl;

        // Descramble the data
        // std::vector<unsigned char> descrambled(num_data_bytes+1, 0);
        // int state = 93, feedback = 0;
        // for(int x = 0; x < num_data_bytes; x++)
        // {
        //    feedback = (!!(state & 64)) ^ (!!(state & 8));
        //    descrambled[x] = feedback ^ decoded[x];
        //    state = ((state << 1) & 0x7E) | feedback;
        // }
        // decoded.swap(descrambled);

        // Calculate the CRC
        boost::crc_32_type crc;
        crc.process_bytes(&decoded[0], m_length);
        unsigned int calculated_crc = crc.checksum();
        unsigned int given_crc = 0;
        memcpy(&given_crc, &decoded[m_length], 4);

        // Verify the CRC
        if(given_crc != calculated_crc)
        {
            std::cerr << "Invalid CRC (length " << m_length << "), Calculated CRC = " << calculated_crc << std::endl;
            // Indicate failure
            return false;
        }
        else
        {
            // Copy the payload
            payload.resize(m_length);
            memcpy(&payload[0], &decoded[0], m_length);
            return true;
        }

    }

}

