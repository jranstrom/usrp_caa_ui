#ifndef UHD_CLIB_HEADER_H
#define UHD_CLIB_HEADER_H

#include "../circbuffer.h"
#include <fstream>
#include <iostream>
#include <uhd/usrp/multi_usrp.hpp>
#include <bitset>
#include <fftw3.h>
#include <cmath>
#include <numbers>

class uhd_clib{
public:
    uhd_clib(); // constructor

static std::complex<short> conv_double2short(std::complex<double> & in_val, double mx = 0);


static std::vector<std::vector<int>> transpose_int_mat(std::vector<std::vector<int>> mat_in);

static std::vector<std::complex<short>> cvec_conv_double2short(std::vector<std::complex<double>> & in_vec);

static std::vector<std::complex<double>> cvec_conv_short2double(std::vector<std::complex<short>> & in_vec, double mx = std::pow(2,(16-1)));

template<typename samp_type>
static void read_file_into_buffer(CircBuffer<samp_type> & c_buff,
                            const std::string& file,
                            size_t samps_per_buff = 13);

static double get_max(std::vector<double> & in_v,int & loc);

static std::vector<double> find_peaks(std::vector<double> & in, 
                                    std::vector<int> & locs, 
                                    double min_value = 0, 
                                    int min_dist = 0, 
                                    int min_num = 0);

static void print_transmitter_config(uhd::usrp::multi_usrp::sptr tx_usrp,std::string ip);
static void print_receiver_config(uhd::usrp::multi_usrp::sptr rx_usrp,std::string ip);

static std::vector<bool> encodeCRC(std::vector<bool> data, const std::vector<bool> & crcPoly);
static bool checkValidCRC(std::vector<bool> enc_data, const std::vector<bool> & crcPoly);

static std::vector<double> fft_correlation_w_ref(std::vector<std::complex<double>> &reference, std::vector<std::complex<double>> &signal);

static std::vector<std::complex<double>> fft_w_zpadd(std::vector<std::complex<double>> &signal, size_t N,bool conjugate=false);

template<typename T, typename A>
static void fft_shift(std::vector<T,A> & signal){
    std::vector<T,A> res_vec = signal;
    size_t N = signal.size();
    int N2 = std::ceil(N/2);

    for(int i=0;i<static_cast<int>(N);i++){
        int k = ((i+N2) % N);
        signal[i] = res_vec[k];
    }
}

template<typename T, typename A>
static std::vector<T,A> extractByIndex(std::vector<T,A> const& input,std::vector<int> index_vec){
    std::vector<T,A> result_vec(index_vec.size());
    for(size_t i=0;i<index_vec.size();i++){
        result_vec[i] = input[index_vec[i]];
    }
}

static double angle(std::complex<double> const& iq_value);
static std::vector<double> angle(std::vector<std::complex<double>> const& iq_values);
static double rad2deg(double const& phase);
static std::vector<double> rad2deg(std::vector<double> const& phases);

static std::complex<short> CalculateComplexMean(std::vector<std::complex<short>> input);

template<typename T,typename A>
static void scalarAddition(std::vector<T,A> & input,T & scalar){
    size_t N = input.size();
    for(size_t i=0;i<N;i++){
        input[i] = input[i]+scalar;
    }
}
template<typename T,typename A>
static void scalarSubtraction(std::vector<T,A> & input,T & scalar){
    size_t N = input.size();
    for(size_t i=0;i<N;i++){
        input[i] = input[i]-scalar;
    }
}

private :

};

#endif
