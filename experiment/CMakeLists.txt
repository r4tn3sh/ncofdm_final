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

list(APPEND test_nc_sim_srcs
        test_nc_sim.cpp
)

list(APPEND test_tx_srcs
	test_tx.cpp
)

list(APPEND test_rx_srcs
        test_rx.cpp
)
#
#########################################################################
## Create executables
#########################################################################
add_executable(sim ${test_sim_srcs})
add_executable(nc_sim ${test_nc_sim_srcs})
add_executable(test_tx ${test_tx_srcs})
add_executable(test_rx ${test_rx_srcs})
#
#
#########################################################################
## Link executables to wno_ofdm library
#########################################################################
target_link_libraries(sim wno_ofdm)
target_link_libraries(nc_sim wno_ofdm)
target_link_libraries(test_tx wno_ofdm)
target_link_libraries(test_rx wno_ofdm)

