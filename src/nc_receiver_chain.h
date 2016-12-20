/*! \file nc_receiver_chain.h
 *  \brief Header file for Receiver Chain class
 *
 *  The Receiver Chain class is the main controller for the blocks that are
 *  used to receive and decode PHY layer frames. It holds the instances of each block
 *  and shifts the data through the receive chain as it is processed eventually returning
 *  the correctly received payloads (MPDUs) which can then be passed to the upper layers.
 */

#ifndef NC_RECEIVER_CHAIN_H
#define NC_RECEIVER_CHAIN_H

#include <thread>
#include <semaphore.h>

#include "fft_symbols.h"
#include "block.h"
#include "tagged_vector.h"
#include "basic_nc_frame_decoder.h"
#include "underlay_tagged_decode.h"

namespace wno
{

    /*! \brief The Receiver Chain class.
     *
     *  Inputs raw complex doubles representing the base-band digitized time domain signal.
     *
     *  Outputs vector of correctly received payloads (MPDUs) which are themselves vectors
     *  of unsigned chars.
     *
     *  The Receiver Chain class is the main controller for the blocks that are
     *  used to receive and decode PHY layer frames. It holds the instances of each block
     *  and shifts the data through the receive chain as it is processed eventually returning
     *  the correctly received payloads (MPDUs) which can then be passed to the upper layers.
     */
    class nc_receiver_chain
    {
    public:

        /*!
         * \brief Constructor for nc_receiver_chain
         */
        nc_receiver_chain(uint64_t sc_map);
        // nc_receiver_chain();

        /*!
         * \brief Processes the raw time domain samples.
         * \param samples A vector of received time-domain samples from the usrp block to pass to
         *  the receive chain for signal processing.
         * \return A vector of correctly received payloads where each payload is its own vector
         *  of unsigned chars.
         */
        std::vector<std::vector<unsigned char> > process_samples(std::vector<std::complex<double> > samples);

    private:

        /**********
         * Blocks *
         **********/

        underlay_tagged_decode * m_ul_tagged_decoder;
        fft_symbols    * m_fft_symbols;        //!< Forward FFT of symbols
        basic_nc_frame_decoder  * m_nc_frame_decoder;      //!< Frame decoding

        /***********************************
         * Scheduler Variables and Methods *
         ***********************************/

        /*!
         * \brief Adds block to the receiver call chain
         * \param block A pointer to the block so that its work function can be called
         */
        void add_block(wno::block_base * block);

        /*!
         * \brief Runs the block by calling its work function
         * \param index the block's index for referencing the correct semaphores for that block.
         * \param block A pointer to the block used as a handle to access its work() function.
         */
        void run_block(int index, wno::block_base * block);

        uint64_t m_sc_map;

        std::vector<std::thread> m_threads; //!< Vector of threads - one for each block


        std::vector<sem_t> m_wake_sems; //!< Vector of semaphores used to "wake up" each block


        std::vector<sem_t> m_done_sems; //!< Vector of semaphores used to determine when the blocks are done
    };

}

#endif // NC_RECEIVER_CHAIN_H
