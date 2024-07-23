#include "uhd_clib.h"
#include "../circbuffer.h"
#include <fstream>
#include <iostream>
#include <uhd/usrp/multi_usrp.hpp>


uhd_clib::uhd_clib(){}

std::complex<short> uhd_clib::conv_double2short(std::complex<double> & in_val, double mx){

    if(mx != 0){
        mx = std::pow(2,(16-1))/mx;
    }else{
        mx = 1;
    }

    return std::complex<short>(static_cast<short>(real(in_val)*mx),static_cast<short>(imag(in_val*mx)));}

std::vector<std::vector<int>> uhd_clib::transpose_int_mat(std::vector<std::vector<int>> mat_in){
    size_t n_rows = mat_in.size();
    size_t n_cols = mat_in[0].size();

    std::vector<std::vector<int>> mat_out(n_cols,std::vector<int>(n_rows));

    for(int r=0;r<n_rows; r++){
        for(int c=0;c<n_cols;c++){
            mat_out[c][r] = mat_in[r][c];
        }
    }
    return mat_out;
}

std::vector<std::complex<short>> uhd_clib::cvec_conv_double2short(std::vector<std::complex<double>> & in_vec){
    std::vector<std::complex<short>> out_vec(in_vec.size());

    double max_val = 0;
    for(size_t i=0;i<out_vec.size();i++){
        max_val = std::max(max_val,std::max(abs(real(in_vec[i])),abs(imag(in_vec[i]))));
    }

    double mx = std::pow(2,(16-1))/max_val;


    for(size_t i=0;i<out_vec.size();i++){
        out_vec[i] = std::complex<short>(static_cast<short>(real(in_vec[i])*mx),static_cast<short>(imag(in_vec[i]*mx)));
    }

    return out_vec;}

std::vector<std::complex<double>> uhd_clib::cvec_conv_short2double(std::vector<std::complex<short>> & in_vec, double mx){
    std::vector<std::complex<double>> out_vec(in_vec.size());

    if(mx == -1){
        double max_val = 0;
        for(size_t i=0;i<out_vec.size();i++){
            short real_amp = abs(real(in_vec[i]));
            short imag_amp = abs(imag(in_vec[i]));

            double comp_val;

            if(real_amp > imag_amp){
                comp_val = static_cast<double>(real_amp);
            }else{
                comp_val = static_cast<double>(imag_amp);
            }

            if(comp_val > max_val){
                max_val = comp_val;
            }
        }

        mx = std::pow(2,(16-1))/max_val;
    }
    //double mx = std::pow(2,(16-1));


    for(size_t i=0;i<out_vec.size();i++){
        out_vec[i] = std::complex<double>(static_cast<double>(real(in_vec[i])),static_cast<double>(imag(in_vec[i]))) / mx;
    }

    return out_vec;}

void uhd_clib::print_transmitter_config(uhd::usrp::multi_usrp::sptr tx_usrp,std::string ip){
    std::cout << ">> Transmitter Settings" << std::endl;
    std::cout << "\tIp adress: " << ip << std::endl;
    std::cout << "\tSample rate: " << tx_usrp->get_tx_rate()/1e6 << " Msps" << std::endl;
    std::cout << "\tCarrier frequency: " << tx_usrp->get_tx_freq()/1e9 << " GHz" << std::endl;
    std::cout << "\tGain: " << tx_usrp->get_tx_gain() << " dB" << std::endl;
    std::cout << "\tBandwidth: " << tx_usrp->get_tx_bandwidth()/1e6 << " MHz" << std::endl;
    std::cout << "\tAntenna: " << tx_usrp->get_tx_antenna() << "" << std::endl;
    std::cout << "\tPPS: " << tx_usrp->get_time_source(0) << "" << std::endl;
    std::cout << "\tREF: " << tx_usrp->get_clock_source(0) << "" << std::endl;
}

void uhd_clib::print_receiver_config(uhd::usrp::multi_usrp::sptr rx_usrp,std::string ip){
    std::cout << ">> Receiver Settings" << std::endl;
    std::cout << "\tIp adress: " << ip << std::endl;
    std::cout << "\tSample rate: " << rx_usrp->get_rx_rate()/1e6 << " Msps" << std::endl;
    std::cout << "\tCarrier frequency: " << rx_usrp->get_rx_freq()/1e9 << " GHz" << std::endl;
    std::cout << "\tGain: " << rx_usrp->get_rx_gain() << " dB" << std::endl;
    std::cout << "\tBandwidth: " << rx_usrp->get_rx_bandwidth()/1e6 << " MHz" << std::endl;
    std::cout << "\tAntenna: " << rx_usrp->get_rx_antenna() << "" << std::endl;
    std::cout << "\tPPS: " << rx_usrp->get_time_source(0) << "" << std::endl;
    std::cout << "\tREF: " << rx_usrp->get_clock_source(0) << "" << std::endl;
}

std::vector<bool> uhd_clib::encodeCRC(std::vector<bool> data, const std::vector<bool> &crcPoly)
{
    int M = crcPoly.size();
    size_t N = data.size();
    std::vector<bool> result = data;

    for (int i=0;i<M-1;++i){
        data.push_back(0);
    }

    int ix=0;
    while(ix < N-1){
        // find leading '1'
        bool leadingOne_found = false;
        while(!leadingOne_found && ix<N){
            if(data[ix]){
                leadingOne_found = true;
            }else{
                ++ix;
            }
        }
        if(leadingOne_found){
            for(int i=0; i<M;++i){
                // xor
                data[i+ix] = (data[i+ix] != crcPoly[i]);
            }
        }
    }
    for(int i=0;i<M-1;i++){
        result.push_back(data[N+i]);
    }
    return result;
}

bool uhd_clib::checkValidCRC(std::vector<bool> enc_data, const std::vector<bool> &crcPoly)
{
    int M = crcPoly.size();
    size_t N = enc_data.size()-M+1;

    int ix=0;
    while(ix < N-1){
        // find leading '1'
        bool leadingOne_found = false;
        while(!leadingOne_found && ix<N){
            if(enc_data[ix]){
                leadingOne_found = true;
            }else{
                ++ix;
            }
        }
        if(leadingOne_found){
            for(int i=0; i<M;++i){
                // xor
                enc_data[i+ix] = (enc_data[i+ix] != crcPoly[i]);
            }
        }
    }

    bool valid = true;
    for(int i=0;i<M-1;i++){
        if(enc_data[N+i]){
            valid = false;
        }
    }
    return valid;
}

std::vector<std::complex<double>> uhd_clib::fft_w_zpadd(std::vector<std::complex<double>> &signal,size_t N,bool conjugate)
{
    std::vector<std::complex<double>> result_vec(N);
    size_t sig_size = signal.size();

    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    for(int i=0;i<N;i++){
        if(i<sig_size){
            in[i][0] = std::real(signal[i]);
            in[i][1] = std::imag(signal[i]);
        }else{
            in[i][0] = 0.0;
            in[i][1] = 0.0;
        }
    }

    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    if(!conjugate){
        for(int i=0;i<N;i++){
            result_vec[i] = std::complex(out[i][0],out[i][1]);
        }
    }else{
        for(int i=0;i<N;i++){
            result_vec[i] = std::complex(out[i][0],-out[i][1]);
        }
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    return result_vec;



}

double uhd_clib::angle(std::complex<double> const& iq_value)
{
    return std::imag(std::log(iq_value));
}

std::vector<double> uhd_clib::angle(const std::vector<std::complex<double> > &iq_values)
{
    std::vector<double> result_vec(iq_values.size());

    for(size_t i =0;i<iq_values.size();i++){
        result_vec[i] = angle(iq_values[i]);
    }

    return result_vec;
}

double uhd_clib::rad2deg(const double &phase)
{
    return phase / std::numbers::pi *180;
}

std::vector<double> uhd_clib::rad2deg(const std::vector<double> &phases)
{
    std::vector<double> result_vec(phases.size());
    for(size_t i=0;i<phases.size();i++){
        result_vec[i] = rad2deg(phases[i]);
    }

    return result_vec;
}

std::complex<short> uhd_clib::CalculateComplexMean(std::vector<std::complex<short>> input)
{
    std::complex<short> result =std::complex(0,0);
    for(size_t i=0;i<input.size();i++){
            result = result+input[i];
    }

    short K = static_cast<short>(input.size());

    return std::complex(std::real(result)/K,std::imag(result)/K);
}

std::vector<double> uhd_clib::fft_correlation_w_ref(std::vector<std::complex<double> > &reference, std::vector<std::complex<double> > &signal)
{
    // It is expected that reference is already zero-padded, fft and complex conjugate

    size_t N  = signal.size(); //get signal length
    std::vector<double> result_vec(N);

    fftw_complex *sig, *sig_fft, *conv_fft,*conv_ifft;
    fftw_plan p, p_inv;

    // Allocate resources
    sig = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    sig_fft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    conv_fft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    conv_ifft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    // Load sig buffer with signal values
    for(int i =0;i<N;i++){
        sig[i][0] = std::real(signal[i]);
        sig[i][1] = std::imag(signal[i]);
    }

    // Perform FFT of signal
    p = fftw_plan_dft_1d(N, sig, sig_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    // Multiply with reference
    for(int i=0;i<N;i++){
        conv_fft[i][0] = sig_fft[i][0] * std::real(reference[i])-sig_fft[i][1] * std::imag(reference[i]);
        conv_fft[i][1] = sig_fft[i][0] * std::imag(reference[i]) + sig_fft[i][1] * std::real(reference[i]);
    }

    p_inv = fftw_plan_dft_1d(N,conv_fft,conv_ifft,FFTW_BACKWARD,FFTW_ESTIMATE);

    fftw_execute(p_inv);

    // Get power
    for(int i=0;i<N;i++){
        result_vec[i] = std::pow(conv_ifft[i][0],2) + std::pow(conv_ifft[i][1],2);
    }


    // Clean up
    fftw_destroy_plan(p);
    fftw_destroy_plan(p_inv);
    fftw_free(sig);
    fftw_free(sig_fft);
    fftw_free(conv_fft);
    fftw_free(conv_ifft);

    return result_vec;
}

template<typename samp_type>
void uhd_clib::read_file_into_buffer(CircBuffer<samp_type> &c_buff,
                            const std::string& file,
                            size_t samps_per_buff){
    std::vector<samp_type> buff(samps_per_buff);
    std::ifstream infile(file.c_str(),std::ifstream::binary);
    bool not_reached_end = true;
    while(not_reached_end){
        infile.read((char*)&buff.front(),buff.size()*sizeof(samp_type));
        size_t num_tx_samps = size_t(infile.gcount()/sizeof(samp_type));
        for(int i=0;i < num_tx_samps; i++){
            c_buff.push(buff[i]);
        }
        if (infile.eof() || num_tx_samps < samps_per_buff){           
            not_reached_end = false;
        }
    }}


double uhd_clib::get_max(std::vector<double> & in_v,int & loc){
    double max_val = in_v[1];
    loc = 0;
    for(int ii=1;ii<in_v.size();ii++){
        if(in_v[ii] > max_val){
            loc = ii;
            max_val = in_v[ii];
        }
    }

    return max_val;}

std::vector<double> uhd_clib::find_peaks(std::vector<double> & in, 
                                    std::vector<int> & locs, 
                                    double min_value, 
                                    int min_dist, 
                                    int min_num){
    std::vector<double> res_vec_val = in;
    std::vector<int> res_vec_ind;

    for(int ii=0;ii<res_vec_val.size();++ii){
        res_vec_ind.push_back(ii);
    }

    // is a peak?
    std::vector<double> res_is_peak_val;
    std::vector<int> res_is_peak_i;
    for(int ii=1;ii<res_vec_val.size();++ii){
        if(res_vec_val[ii] > res_vec_val[ii-1] && res_vec_val[ii] > res_vec_val[ii+1]){
            res_is_peak_val.push_back(res_vec_val[ii]);
            res_is_peak_i.push_back(res_vec_ind[ii]);
        }
    }

    res_vec_val = res_is_peak_val;
    res_vec_ind = res_is_peak_i;

    if(min_value > 0){

        std::vector<double> res_min_val;
        std::vector<int> res_min_val_i;

        for(int i=0;i<res_vec_val.size();i++){
            if(res_vec_val[i] >= min_value){

                res_min_val.push_back(res_vec_val[i]);
                res_min_val_i.push_back(res_vec_ind[i]);
            }
        }

        res_vec_val = res_min_val;
        res_vec_ind = res_min_val_i;
    }

    bool mn = false;
    if(min_num > 0){
        mn = true;
    }

    if(min_dist > 0){

        std::vector<double> res_min_dist_val;
        std::vector<int>  res_min_dist_i;
        int fnd = 0;
        while(res_vec_val.size() > 0 and (not mn or fnd < min_num)){

            int tmp_lo;
            double max_val = get_max(res_vec_val,tmp_lo);
            int max_lo = res_vec_ind[tmp_lo];

            ++fnd;  // found another peak

            res_min_dist_val.push_back(max_val);
            res_min_dist_i.push_back(max_lo);


            std::vector<double> tmp_md_val;
            std::vector<int> tmp_md_i;

            for(int ii=0;ii<res_vec_ind.size();ii++){
                if(abs(max_lo-res_vec_ind[ii]) >= min_dist){

                    tmp_md_val.push_back(res_vec_val[ii]);
                    tmp_md_i.push_back(res_vec_ind[ii]);
                }
            }

            res_vec_val = tmp_md_val;
            res_vec_ind = tmp_md_i;
        }

        res_vec_val = res_min_dist_val;
        res_vec_ind = res_min_dist_i; 
        
    }


    // sort
    locs = res_vec_ind;
    std::vector<double> c_res_vec_val = res_vec_val;

    std::sort(locs.begin(), locs.end());

    for(int i=0;i<locs.size();i++){
        bool n_fnd = true;
        int k = 0;
        while(n_fnd){
            if(locs[i] == res_vec_ind[k++]){
                n_fnd = false;
                res_vec_val[i] == c_res_vec_val[k-1];
            }            
        }
    }

    return res_vec_val;
}
