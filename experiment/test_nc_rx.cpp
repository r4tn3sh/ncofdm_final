/*! \file test_nc_rx.cpp
 *  \brief nc_receiver test
 *
 *  This file is used to test receiving OFDM PHY frames over the air.
 */


#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/program_options.hpp>
#include "nc_receiver.h"

using namespace wno;

void test_nc_rx(double freq, double sample_rate, double rx_gain);
void test_nc_rx_pause(double freq, double rate, double rx_gain);
void process_packets_callback(std::vector<std::vector<unsigned char> > packets);
bool set_realtime_priority();

double freq = 5.72e9;
double sample_rate = 5e6;
//double tx_gain = 30;
double rx_gain = 30;
//double amp = 0.5;
//Rate phy_rate = RATE_1_2_BPSK;
uint64_t SC_MAP = 71777218305396495; //00000ffffffff000

int rx_count = 0; //!< Total number of packets received since the nc_receiver was activated

int main(int argc, char * argv[]){

    std::cout << "Testing receive chain..." << std::endl;

    test_nc_rx(freq, sample_rate, rx_gain);

//    test_nc_rx_pause(freq, sample_rate, rx_gain);

    return 0;
}


/*!
 * \brief Function for testing the fun_ofdm nc_receiver
 * \param freq Center Frequency
 * \param sample_rate Sample Rate
 * \param rx_gain nc_receiver Gain
 *
 *  This function creates a nc_receiver object which spawns its own thread for the nc_receiver chain.
 *  It passes a pointer to the  #process_packets_callback() function which is used to process
 *  the received packets (in this case it just counts them). This function then proceeds to spin
 *  forever in a while(1) loop until the user kills the program externally (preferably by using
 *  something like Ctrl+c or the kill command in the shell, however, while not suggested,
 *  pulling the power plug does also effectively kill the program).
 */
void test_nc_rx(double freq, double sample_rate, double rx_gain)
{

    set_realtime_priority();

    // Instantiate a usrp
    printf("Instantiating the usrp.\n");

    nc_receiver rx = nc_receiver(&process_packets_callback, SC_MAP, freq, sample_rate, rx_gain, "");

    while(1);
}


/*!
 * \brief Function for testing the fun_ofdm nc_receiver and demonstrating the use of the
 * nc_receiver::pause() and nc_receiver::resume() functions.
 * \param freq Center Frequency
 * \param sample_rate Sample Rate
 * \param rx_gain nc_receiver Gain
 *
 *  This function is similar to test_nc_rx() function, however instead of running an empty
 *  while(1) loop, it instead runs a while(1) loop that sleeps for 4 seconds allowing the
 *  nc_receiver to run freely. It then pauses the nc_receiver for 1 second using the
 *  nc_receiver::pause() function, waits 1 second, then resumes the nc_receiver by using the
 *  nc_receiver::resume() function.
 */
void test_nc_rx_pause(double freq, double sample_rate, double rx_gain)
{

    // Instantiate a usrp
    printf("Instantiating the usrp.\n");
    nc_receiver rx = nc_receiver(&process_packets_callback, SC_MAP, freq, sample_rate, rx_gain, "");

    while(1)
    {
        sleep(4);

        std::cout << "Pausing nc_receiver for 1 second " << std::endl;
        rx.pause();

        sleep(1);

        std::cout << "Resuming the nc_receiver" << std::endl;
        rx.resume();

    }
}


/*!
 * \brief The callback function for processing the Received Packets
 * \param packets The successfully received packets in from the nc_receiver_chain
 *
 *  This function merely counts the number of received packets and prints the timestamps
 *  of when the packets were received.
 */
void process_packets_callback(std::vector<std::vector<unsigned char> > packets)
{
    rx_count += packets.size();

    boost::posix_time::ptime rx_time = boost::posix_time::microsec_clock::local_time();
    if(packets.size() > 0)
    {
        std::cout << "Received " << rx_count << " packets at " << rx_time.time_of_day() << std::endl;
    }

}

/*!
 * \brief Attempt to set real time priority for thread scheduling
 * \return Whether or not real time priority was succesfully set.
 */
bool set_realtime_priority()
{
    // Get the current thread
    pthread_t this_thread = pthread_self();

    // Set priority to SCHED_FIFO
    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_RR);
    if (pthread_setschedparam(this_thread, SCHED_RR, &params) != 0)
    {
        std::cout << "Unable to set realtime priority. Did you forget to sudo?" << std::endl;
        return false;
    }

    return true;
}

