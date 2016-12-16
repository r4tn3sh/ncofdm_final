/*! \file underlay_tagged_decode.h
 *  \brief Hader file for the Timing Sync block.
 *
 *  The timing sync block is in charge of using the two LTS symbols to align the received frame in time.
 *  It also uses the two LTS symbols to perform an initial frequency offset estimation and
 *  applying the necessary correction.
 */

#ifndef UNDERLAY_TAGGED_DECODE_H
#define UNDERLAY_TAGGED_DECODE_H
#include <complex>

#include "block.h"
#include "tagged_vector.h"

namespace wno
{
    /*!
     * \brief The underlay_tagged_decode block.
     */
    class underlay_tagged_decode : public wno::block<std::complex<double>, tagged_sample >
    {
    public:

        underlay_tagged_decode(); //!< Constructor for underlay_tagged_decode block.

        virtual void work(); //!< Signal processing happens here.

    private:
        double correlate(std::vector<std::complex<double> >);
        int prev_bit = 0;
        int prev_conf = 0;
        int bits_in_error = 0;
        std::vector<std::complex<double> > m_carryover;
    };
}

#endif // UNDERLAY_TAGGED_DECODE_H
