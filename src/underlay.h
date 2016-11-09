/*! \file underlay.h
 *  \brief Header file for the underlay.
 */

#include <complex>
#include <vector>

#ifndef UNDERLAY_H
#define UNDERLAY_H

#endif // UNDERLAY_H

namespace wno
{
    /*!
     * Some info about the underlay block
     */
    static std::complex<double> SPNS[64] =
    {
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 )
    };
    static int SPNRep = 4;

    static std::complex<double> LPNS[64] =
    {
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>(-1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 1,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 ),
        std::complex<double>( 0,  0 )
    };

    class underlay
    {
        public:
            underlay();
            std::vector<std::complex<double> > add_underlay(std::vector<std::complex<double> > overlay_data);
    };
}
