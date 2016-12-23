/*! \file nc_receiver.h
 *  \brief Header file for the nc_receiver class.
 *
 *  The nc_receiver class is the public interface for the wno_ofdm nc_receiver.
 *  This is the easiest way to start receiving 802.11a OFDM frames out of the box.
 */

#ifndef NC_nc_receiver_H
#define NC_nc_receiver_H

#include <semaphore.h>
#include <vector>
#include "nc_receiver_chain.h"
#include "usrp.h"

#define NUM_RX_SAMPLES 1600

namespace wno
{

    /*!
     * \brief The nc_receiver class is the public interface for the wno_ofdm nc_receiver.
     *
     *  Usage: To receive packets simply create a nc_receiver object and pass it a callback
     *  function that takes a std::vector<std::vector<unsigned char> > as an input parameter.
     *  The nc_receiver object then automatically creates a separate thread that pulls samples
     *  from the USRP and processes them with the receive chain. The received packets (if any)
     *  are then passed into the callback function where the user is able to process them further.
     *
     *  If at any time the user wishes to pause the nc_receiver (i.e. so that the user can transmit
     *  some packets) the user simply needs to call the nc_receiver::pause() function on the nc_receiver
     *  object. Similarly, the nc_receiver::resume() function can then be used to begin receiving again
     *  after a pause.
     */
    class nc_receiver
    {
    public:

        /*!
         * \brief Constructor for the nc_receiver with raw parameters
         * \param callback Function pointer to the callback function where received packets are passed
         * \param freq [Optional] Center frequency
         * \param samp_rate [Optional] Sample Rate
         * \param rx_gain [Optional] Receive Gain
         * \param device_addr [Optional] IP address of USRP device
         *
         *  Defaults to:
         *  - center freq -> 5.72e9 (5.72 GHz)
         *  - sample rate -> 5e6 (5 MHz)
         *  - rx gain -> 20
         *  - device ip address -> "" (empty string will default to letting the UHD api
         *    automatically find an available USRP)
         *  - *Note:
         *    + tx_gain -> 20 even though it is irrelevant for the nc_receiver
         *    + amp -> 1.0 even though it is irrelevant for the nc_receiver
         */
        nc_receiver(void(*callback)(std::vector<std::vector<unsigned char> > packets), uint64_t sc_map, double freq = 5.72e9, double samp_rate = 5e6, double rx_gain = 20, std::string device_addr = "");

        /*!
         * \brief Constructor for the nc_receiver that uses the usrp_params struct
         * \param callback Function pointer to the callback function where received packets are passed
         * \param params [Optional] The usrp parameters you want to use for this nc_receiver.
         *
         *  Defaults to:
         *  - center freq -> 5.72e9 (5.72 GHz)
         *  - sample rate -> 5e6 (5 MHz)
         *  - tx gain -> 20
         *  - rx gain -> 20 (although this is irrelevant for the transmitter)
         *  - device ip address -> "" (empty string will default to letting the UHD api
         *    automatically find an available USRP)
         */
        nc_receiver(void(*callback)(std::vector<std::vector<unsigned char> > packets), usrp_params params = usrp_params());

        /*!
         * \brief Pauses the nc_receiver thread.
         */
        void pause();

        /*!
         * \brief Resumes the nc_receiver thread after it has been paused.
         */
        void resume();

    private:

        void nc_receiver_chain_loop(); //!< Infinite while loop where samples are received from USRP and processed by the nc_receiver_chain

        void (*m_callback)(std::vector<std::vector<unsigned char> > packets); //!< Callback function pointer

        usrp m_usrp; //!< The usrp object used to nc_receiver frames over the air

        nc_receiver_chain m_rec_chain; //!< The nc_receiver chain object used to detect & decode incoming frames

        std::vector<std::complex<double> > m_samples; //!< Vector to hold the raw samples received from the USRP and passed into the nc_receiver_chain

        std::thread m_rec_thread; //!< The thread that the nc_receiver chain runs in

        sem_t m_pause; //!< Semaphore used to pause the nc_receiver thread




    };


}

#endif // NC_nc_receiver_H
