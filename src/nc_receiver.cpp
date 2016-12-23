/*! \file nc_receiver.h
 *  \brief C++ file for the nc_receiver class.
 *
 *  The nc_receiver class is the public interface for the wno_ofdm nc_receiver.
 *  This is the easiest way to start receiving 802.11a OFDM frames out of the box.
 */

#include "nc_receiver.h"

namespace wno
{

    /*!
     * This constructor shows exactly what parameters need to be set for the nc_receiver.
     */
    nc_receiver::nc_receiver(void (*callback)(std::vector<std::vector<unsigned char> > packets), uint64_t sc_map, double freq, double samp_rate, double rx_gain, std::string device_addr) :
        nc_receiver(callback, usrp_params(freq, samp_rate, 20, rx_gain, 1.0, device_addr))
    {
        m_rec_chain.set_sc_map(sc_map);
    }

    /*!
     * This constructor is for those who feel more comfortable using the usrp_params struct.
     */
    nc_receiver::nc_receiver(void (*callback)(std::vector<std::vector<unsigned char> > packets), usrp_params params) :
        m_usrp(params),
        m_samples(NUM_RX_SAMPLES),
        m_callback(callback)
    {
        sem_init(&m_pause, 0, 1); //Initial value is 1 so that the nc_receiver_chain_loop() will begin executing immediately
        m_rec_thread = std::thread(&nc_receiver::nc_receiver_chain_loop, this); //Initialize the main nc_receiver thread
    }

    /*!
     *  This function loops forever (unless it is paused) pulling samples from the USRP and passing them through the
     *  nc_receiver chain. It then passes any successfully decoded packets to the callback function for the user
     *  to process further. This function can be paused by the user by calling the nc_receiver::pause() function,
     *  presumably so that the user can transmit packets over the air using the transmitter. Once the user is finished
     *  transmitting he/she can resume the nc_receiver by called the nc_receiver::resume() function. These two functions use
     *  an internal semaphore to block the nc_receiver code execution while in the paused state.
     */
    void nc_receiver::nc_receiver_chain_loop()
    {
        while(1)
        {
            sem_wait(&m_pause); // Block if the nc_receiver is paused

            m_usrp.get_samples(NUM_RX_SAMPLES, m_samples);

            std::vector<std::vector<unsigned char> > packets =
                    m_rec_chain.process_samples(m_samples);

            m_callback(packets);

            sem_post(&m_pause); // Flags the end of this loop and wakes up any other threads waiting on this semaphore
                                // i.e. a call to the pause() function in the main thread.
        }
    }

    /*!
     *  Uses an internal semaphore to block the execution of the nc_receiver loop code effectively pausing
     *  the nc_receiver until the semaphore is posted to (cleared) by the nc_receiver::resume() function.
     */
    void nc_receiver::pause()
    {
        sem_wait(&m_pause);
    }

    /*!
     *  This function posts to (clears) the internal semaphore that is blocking the nc_receiver loop code execution
     *  due to a previous call to the nc_receiver::pause() function, thus allowing the main nc_receiver loop to begin
     *  executing again.
     */
    void nc_receiver::resume()
    {
        sem_post(&m_pause);
    }
}
