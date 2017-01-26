# ncofdm_final
Implementation of NC-OFDM using C++ API of UHD.
### Download
git clone https://github.com/r4tn3sh/ncofdm_final.git -b underlay
### Installation
cd ncofdm_final

mkdir build

cd build

cmake ../

make
### Running code
Once you run the *make*, there should be a *bin* subfolder within *ncofdm_final* folder. This subfolder contains executable binaries. Currently it contains following examples
#### ./nc_sim
Runs a simulation where the transmitter sends NC-OFDM symbols over a given set of subcarriers. An underlay channel has been added to help with timing, which helps in identfying the start of the frame.
#### ./test_nc_tx
Basic NC-OFDM transmitter. Currently has not been tested completely. 
#### ./test_nc_rx
Basic NC-OFDM receiver. Currently has not been tested completely. 
