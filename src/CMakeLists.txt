########################################################################
# Src CMakeLists.txt
#
#
########################################################################



########################################################################
# List Header .h & Source .cpp Files
########################################################################

# Not necessary to list headers, but helps QtCreator load them properly in sources window
# Also doesn't hurt anything so why not
list(APPEND headers

    block.h
    circular_accumulator.h
    preamble.h
    qam.h
    rates.h
    tagged_vector.h

    channel_est.h
    fft.h
    fft_symbols.h
    frame_builder.h
    symbol_builder.h
    frame_decoder.h
    frame_detector.h
    interleaver.h
    modulator.h
    parity.h
    phase_tracker.h
    ppdu.h
    puncturer.h
    receiver_chain.h
    symbol_mapper.h
    timing_sync.h
    usrp.h
    viterbi.h

    transmitter.h
    receiver.h
    underlay.h
    underlay_decode.h
)

list(APPEND sources 

    channel_est.cpp
    fft.cpp
    fft_symbols.cpp
    frame_builder.cpp
    symbol_builder.cpp
    frame_decoder.cpp
    frame_detector.cpp
    interleaver.cpp
    modulator.cpp
    parity.cpp
    phase_tracker.cpp
    ppdu.cpp
    puncturer.cpp
    receiver_chain.cpp
    symbol_mapper.cpp
    timing_sync.cpp
    usrp.cpp
    viterbi.cpp

    transmitter.cpp
    receiver.cpp
    underlay.cpp
    underlay_decode.cpp
)


########################################################################
# Build Library wno_ofdm from sources
########################################################################
 add_library(wno_ofdm SHARED ${headers} ${sources})

########################################################################
# Link external dependencies:
#	uhd
#	fftw3
#	boost
#	pthread (this one is a bit strange)
########################################################################
# target_link_libraries(wno_ofdm uhd fftw3 pthread)  # Equivalent to below
 target_link_libraries(wno_ofdm ${UHD_LIBRARIES} ${FFTW3_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}) #equivalent to line above but better style?

########################################################################
# Install Rules
########################################################################

#Install Library (defaults to /usr/local/lib on linux)
install(TARGETS wno_ofdm
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION wno_ofdm)

#Install Header files to system
install(FILES ${headers}
        DESTINATION include/wno_ofdm)
