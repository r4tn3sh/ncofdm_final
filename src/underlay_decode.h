/*! \file underlay_decode.h
 *  \brief Hader file for the Timing Sync block.
 *
 *  The timing sync block is in charge of using the two LTS symbols to align the received frame in time.
 *  It also uses the two LTS symbols to perform an initial frequency offset estimation and
 *  applying the necessary correction.
 */

#ifndef UNDERLAY_DECODE_H
#define UNDERLAY_DECODE_H
#include <complex>

#include "block.h"
#include "tagged_vector.h"

namespace wno
{
    /*!
     * \brief The underlay_decode block.
     */
    class underlay_decode : public wno::block<std::complex<double>, tagged_sample>
    {
    public:

        underlay_decode(); //!< Constructor for underlay_decode block.

        virtual void work(); //!< Signal processing happens here.

    private:
        void correlate(std::complex<double>);

    };
}

#endif // UNDERLAY_DECODE_H
