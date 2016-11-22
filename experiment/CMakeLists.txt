########################################################################
# Examples CMakeLists.txt
#
#
########################################################################

########################################################################
# Include directories so cmake knows where .h files live
########################################################################
include_directories (${PROJECT_SOURCE_DIR}/src)

########################################################################
# Example Sources
########################################################################
list(APPEND test_sim_srcs
        test_sim.cpp
)

list(APPEND test_tx_srcs
	test_tx.cpp
)
#
#list(APPEND test_rx_srcs
#        test_rx.cpp
#)
#
#list(APPEND test_transceiver_srcs
#		simple_transceiver.cpp
#)
#
#list(APPEND tx_nc_srcs
#	tx_nc.cpp
#)
#
#list(APPEND rxtx_nc_srcs
#	rxtx_nc.cpp
#)
#
#########################################################################
## Create executables
#########################################################################
add_executable(sim ${test_sim_srcs})
add_executable(test_tx ${test_tx_srcs})
#add_executable(test_rx ${test_rx_srcs})
#add_executable(transceiver ${test_transceiver_srcs})
#add_executable(tx_nc ${tx_nc_srcs})
#add_executable(rxtx_nc ${rxtx_nc_srcs})
#
#
#########################################################################
## Link executables to wno_ofdm library
#########################################################################
target_link_libraries(test_tx wno_ofdm)
#target_link_libraries(tx_nc wno_ofdm)
#target_link_libraries(rxtx_nc wno_ofdm)
#target_link_libraries(test_rx wno_ofdm)
target_link_libraries(sim wno_ofdm)
#target_link_libraries(transceiver wno_ofdm)

