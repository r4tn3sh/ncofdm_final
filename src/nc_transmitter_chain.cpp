/*! \file nc_transmitter_chain.cpp
 *  \brief C++ file for transmitter chain class.
 *
 *  The transmitter chain class is the main controller for the blocks that are
 *  used to transmit and decode PHY layer frames. It holds the instances of each block
 *  and shifts the data through the transmit chain as it is processed eventually returning
 *  the correctly transmitd payloads (MPDUs) which can then be passed to the upper layers.
 */

#include <iostream>
#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "nc_transmitter_chain.h"

namespace wno
{
    /*!
     * -Initializes each transmitter chain block:
     *  + underlay_tagged_decoder
     *  + fft_symbols
     *  + basic_nc_frame_decode
     *
     *  Adds each block to the transmitter chain.
     */
    nc_transmitter_chain::nc_transmitter_chain()
    {
        m_sc_map = 17592186040320;
        m_ul_tagged_decoder = new underlay_tagged_decode();
        m_fft_symbols = new fft_symbols();
        m_nc_frame_decoder = new basic_nc_frame_decoder(m_sc_map);

        // We use semaphore references, so we don't
        // want them to move to a different memory location
        // if the vectors get resized
        m_wake_sems.reserve(100);
        m_done_sems.reserve(100);

        // Add the blocks to the transmitter chain
        add_block(m_ul_tagged_decoder);
        add_block(m_fft_symbols);
        add_block(m_nc_frame_decoder);
    }
    /*!
     * -Initializes each transmitter chain block:
     *  + underlay_tagged_decoder
     *  + fft_symbols
     *  + basic_nc_frame_decode
     *
     *  Adds each block to the transmitter chain.
     */
    nc_transmitter_chain::nc_transmitter_chain(uint64_t sc_map)
    {
        m_sc_map = sc_map;
        m_ul_tagged_decoder = new underlay_tagged_decode();
        m_fft_symbols = new fft_symbols();
        m_nc_frame_decoder = new basic_nc_frame_decoder(m_sc_map);

        // We use semaphore references, so we don't
        // want them to move to a different memory location
        // if the vectors get resized
        m_wake_sems.reserve(100);
        m_done_sems.reserve(100);

        // Add the blocks to the transmitter chain
        // TODO : Two blocks 
        // 1: underlay encoder
        // 2: NC-OFDM frame builder
        add_block(m_ul_tagged_decoder);
        add_block(m_fft_symbols);
        add_block(m_nc_frame_decoder);
    }

    /*!
     * The #add_block function creates a wake & done semaphore for each block.
     * It then creates a new thread for the block to run in and adds that thread
     * to the thread vector for reference.
     * XXX : This is where threads are created for each block
     */
    void nc_transmitter_chain::add_block(wno::block_base * block)
    {
        m_wake_sems.push_back(sem_t()); // create a wake semaphore
        m_done_sems.push_back(sem_t()); // create a done semaphore
        int index = m_wake_sems.size() - 1; // Get the index of this block
        sem_init(&m_wake_sems[index], 0, 0);// init the semaphore
        sem_init(&m_done_sems[index], 0, 0);// init the semaphore
        m_threads.push_back(std::thread(&nc_transmitter_chain::run_block, this, index, block)); 
    }

    /*!
     * The #run_block function is the main thread for controlling the calls to
     * each block's work function. This function is a forever loops that first waits
     * for the wake_sempahore to post indicating its time for the block to "wake up" and
     * process the data that has just been placed in its input_buffer by running its work()
     * function. Then once, the work() function returns run_block posts to the done sempahore
     * that the block has finished processing everything in the input_buffer. At this point
     * it loops back around and waits for the block to be "woken up" again when the next set
     * of input data is ready.
     */
    void nc_transmitter_chain::run_block(int index, wno::block_base * block)
    {
        while(1)
        {
            sem_wait(&m_wake_sems[index]);

            boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
            block->work();
            boost::posix_time::time_duration elapsed = boost::posix_time::microsec_clock::local_time() - start;

            if(elapsed.total_microseconds() > (2000 / 5e6 * 1e6))
            {
                //std::cout << "! - " <<  block->name << std::endl;
            }

            sem_post(&m_done_sems[index]);
        }
    }

    /*!
     * set the value of sc_map
     */
    void nc_transmitter_chain::set_sc_map(uint64_t sc_map)
    {
        m_sc_map = sc_map;
    }
    /*!
     * This function is the main scheduler for the transmit chain.     
     * */
    std::vector<std::vector<unsigned char> > nc_transmitter_chain::process_samples(std::vector<std::complex<double> > samples)
    {
        //TODO : 
        // 1. Get data from a low rate source (thread 'b')
        // 2. Get data for regular tranmission (thread 'a')
        // 4. Add this signal (Have to match the rate, if already not matched)
        // 3. Send to USRP (should be fast enough to handle the sampling rate)
        // Unlock all the threads associated with all blocks
        for(int x = 0; x < m_wake_sems.size(); x++) sem_post(&m_wake_sems[x]);

        // Wait for the threads to finish
        for(int x = 0; x < m_done_sems.size(); x++) sem_wait(&m_done_sems[x]);
        // Return any completed packets
        return m_nc_frame_decoder->output_buffer;
    }

}
