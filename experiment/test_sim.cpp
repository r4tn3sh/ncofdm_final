/*! \file test_sim.cpp
 *  \brief Simulates the building of packets and sending them through the receive chain.
 *
 *  This file is used to simulate building packets with the frame_builder class and then
 *  sending them through the receive chain.
 */

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/program_options.hpp>
#include "usrp.h"
#include "frame_builder.h"
#include "receiver_chain.h"

using namespace wno;

void test_sim();

double freq = 5.26e9;
double sample_rate = 5e6;
double tx_gain = 30;
double rx_gain = 30;
double amp = 0.5;
//Rate phy_rate = RATE_1_2_BPSK;
//Rate phy_rate = RATE_1_2_QAM16;
//Rate phy_rate = RATE_2_3_QAM64;
Rate phy_rate = RATE_3_4_QAM16;

int main(int argc, char * argv[]){

    std::cout << "Running Simulation..." << std::endl;
    test_sim();

    return 0;
}


/*!
 *  This function builds some packets using the frame builder and sends them through
 *  the receiver chain.  This function does NOT use the transmitter and receiver classes.
 */
void test_sim()
{

    frame_builder * fb = new frame_builder();
    receiver_chain * receiver = new receiver_chain();

    // Generate the data
    std::string data("This is a test string. Beware! it might not reach destination............");
    int repeat = 60;

    // copy the data in payload
    std::vector<unsigned char> payload(data.length()*repeat); //Payload = 1500 bytes
    for(int x = 0; x < repeat; x++) memcpy(&payload[x*data.length()], &data[0], data.length());

    // Build a frame
    std::vector<std::complex<double>> samples = fb->build_frame(payload, phy_rate);

    int pad_length = 0;//samples.size()*1000;

    // Concatenate num_frames frames together
    int num_frames = 10;
    std::cout << "Transmitting " << num_frames << " frame, sample size : " << samples.size() << std::endl;
    std::vector<std::complex<double>> samples_con(samples.size() * num_frames + pad_length);
    for(int x = 0; x < num_frames; x++)
    {
        memcpy(&samples_con[x*samples.size()], &samples[0], samples.size() * sizeof(std::complex<double>));
    }

    //Pad the end with 0's to flush receive chain
    std::vector<std::complex<double> > zeros(pad_length);
    memcpy(&samples_con[num_frames*samples.size()], &zeros[0], zeros.size()*sizeof(std::complex<double>));
    std::cout << samples_con.size() << std::endl;

    boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();

    // Run the samples through the receiver chain

    int chunk_size = 4000;

    int count = 0;
    for(int x = 0; x < samples_con.size(); x += chunk_size)
    {
        int start = x;
        int end = x + chunk_size;
        if(end > samples_con.size()) end = samples_con.size();
        std::vector<std::complex<double> > chunk(&samples_con[start], &samples_con[end]);

        std::vector<std::vector<unsigned char> > rec_frames = receiver->process_samples(chunk);
        count += rec_frames.size();

        if(rec_frames.size()){
            for(int i = 0; i < rec_frames.size(); i++){
                for(int j = 0; j < rec_frames[i].size(); j++)
                    std::cout << rec_frames[i][j];
                std::cout << std::endl << std::endl;
            }
        }
    }

    boost::posix_time::time_duration elapsed = boost::posix_time::microsec_clock::local_time() - start;

    printf("Received %i packets\n", count);

    printf("Time elapsed: %f\n", elapsed.total_microseconds() / 1000.0);
}
