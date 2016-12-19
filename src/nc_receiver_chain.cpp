/*! \file nc_receiver_chain.cpp
 *  \brief C++ file for Receiver Chain class.
 *
 *  The Receiver Chain class is the main controller for the blocks that are
 *  used to receive and decode PHY layer frames. It holds the instances of each block
 *  and shifts the data through the receive chain as it is processed eventually returning
 *  the correctly received payloads (MPDUs) which can then be passed to the upper layers.
 */

#include <iostream>
#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "nc_receiver_chain.h"

namespace wno
{
    /*!
     * -Initializes each receiver chain block:
     *  + underlay_tagged_decoder
     *  + fft_symbols
     *  + basic_nc_frame_decode
     *
     *  Adds each block to the receiver chain.
     */
    nc_receiver_chain::nc_receiver_chain(uint64_t sc_map)
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

        // Add the blocks to the receiver chain
        add_block(m_ul_tagged_decoder);
        add_block(m_fft_symbols);
        add_block(m_nc_frame_decoder);
    }

    /*!
     * The #add_block function creates a wake & done semaphore for each block.
     * It then creates a new thread for the block to run in and adds that thread
     * to the thread vector for reference.
     */
    void nc_receiver_chain::add_block(wno::block_base * block)
    {
        m_wake_sems.push_back(sem_t());
        m_done_sems.push_back(sem_t());
        int index = m_wake_sems.size() - 1;
        sem_init(&m_wake_sems[index], 0, 0);
        sem_init(&m_done_sems[index], 0, 0);
        m_threads.push_back(std::thread(&nc_receiver_chain::run_block, this, index, block));
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
    void nc_receiver_chain::run_block(int index, wno::block_base * block)
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
     * This function is the main scheduler for the receive chain. It takes in raw complex samples
     * from the usrp block and passes them first into the Frame Detector block's input buffer.
     * It then unlocks each of the threads by posting to each block's "wake" semaphore. It then
     * waits for each thread to post that it is done with that call to its work() function.
     * Once all the threads are done it shifts the contents of each blocks output buffer to the input
     * buffer of the next block in the chain and returns the contents of the Frame Decoder's
     * output buffer.
     */
    std::vector<std::vector<unsigned char> > nc_receiver_chain::process_samples(std::vector<std::complex<double> > samples)
    {
        // samples -> sync short in
        // m_frame_detector->input_buffer.swap(samples);
        m_ul_tagged_decoder->input_buffer.swap(samples);

        // Unlock the threads
        for(int x = 0; x < m_wake_sems.size(); x++) sem_post(&m_wake_sems[x]);

        // Wait for the threads to finish
        for(int x = 0; x < m_done_sems.size(); x++) sem_wait(&m_done_sems[x]);

        // Update the buffers
        m_fft_symbols->input_buffer.swap(m_ul_tagged_decoder->output_buffer);
        m_nc_frame_decoder->input_buffer.swap(m_fft_symbols->output_buffer);

        // Return any completed packets
        return m_nc_frame_decoder->output_buffer;
    }

}
