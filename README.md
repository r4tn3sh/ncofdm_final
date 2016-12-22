# ncofdm_final
Implementation of NC-OFDM using C++ API of UHD.
### Installation
cd ncofdm_final

mkdir build

cd build

cmake ../

make
### Running code
Once you run the *make*, there should be a *bin* subfolder within *ncofdm_final* folder. This subfolder contains executable binaries. Currently it contains following examples
##### ./nc_sim
Runs a simulation where the transmitter sends NC-OFDM symbols over a given set of subcarriers. An underlay channel has been added to help with timing, which helps in identfying the start of the frame.
