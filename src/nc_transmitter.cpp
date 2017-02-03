/*! \file transmitter.cpp
 *  \brief C++ file for the transmitter class.
 *
 *  The transmitter class is the public interface for the wno_ofdm transmit chain.
 */

#include "nc_transmitter.h"

namespace wno {

    /*!
     *  This constructor shows exactly what parameters need to be set for the transmitter
     */
    nc_transmitter::nc_transmitter(uint64_t sc_map, double freq, double samp_rate, double tx_gain, double tx_amp, std::string device_addr) :
        m_sc_map(sc_map),
        m_usrp(usrp_params(freq, samp_rate, tx_gain, 20, tx_amp, device_addr)),
        m_nc_frame_builder()
    {
    }

    /*!
     * This construct is for those who feel more comfortable using the usrp_params struct
     */
    nc_transmitter::nc_transmitter(usrp_params params) :
        m_usrp(params),
        m_nc_frame_builder()
    {
    }

    // TODO: Similar to the receiver there should be a tranmitter chain loop 
    // This loop will continuously check for presence of data in input buffer
    // Ideally we need two buffer 1. Data channel 2. Control channel
    /*!
     *  Transmits a single frame, blocking until the frame is sent.
     */
    void nc_transmitter::send_frame(std::vector<unsigned char> payload, Rate phy_rate)
    {
        std::vector<std::complex<double> > samples = m_nc_frame_builder.build_frame(payload, phy_rate, m_sc_map);
        m_usrp.send_burst_sync(samples);
    }

}
