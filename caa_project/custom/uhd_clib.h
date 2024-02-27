#ifndef UHD_CLIB_HEADER_H
#define UHD_CLIB_HEADER_H

#include "../circbuffer.h"
#include <fstream>
#include <iostream>
#include <uhd/usrp/multi_usrp.hpp>

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

private :

};

#endif
