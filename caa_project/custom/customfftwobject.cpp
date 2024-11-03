#include "customfftwobject.h"

customFFTWObject::customFFTWObject() {}

customFFTWObject::~customFFTWObject()
{
    for (int ii = 0; ii < availableSize; ++ii) {
        if(defined[ii]){
            fftw_complex * in = getIn(ii);
            fftw_complex * out = getOut(ii);
            fftw_free(in);
            fftw_free(out);
            fftw_destroy_plan(p[ii]);
        }
    }
}

std::vector<std::complex<double> > customFFTWObject::fft_w_zpadd(std::vector<std::complex<double> > &signal, size_t N, bool conjugate)
{
    std::vector<std::complex<double>> result_vec(N);
    size_t sig_size = signal.size();

    int ik = -1;
    for (int ii = 0; ii < availableSize; ++ii) {
        if(N == fft_sizes[ii] && isIFFT[ii] == false){
            ik = ii;
        }
    }

    if(ik != -1){
        fftw_complex * in = getIn(ik);
        fftw_complex* out = getOut(ik);

        for (int ii = 0; ii < N; ++ii) {
            if(ii<sig_size){
                in[ii][0] = std::real(signal[ii]);
                in[ii][1] = std::imag(signal[ii]);
            }else{
                in[ii][0] = 0.0;
                in[ii][1] = 0.0;
            }
            }

        fftw_execute(p[ik]);

        if(!conjugate){
            for(int ii=0;ii<N;ii++){
                result_vec[ii] = std::complex(out[ii][0],out[ii][1]);
            }
        }else{
            for(int ii=0;ii<N;ii++){
                result_vec[ii] = std::complex(out[ii][0],-out[ii][1]);
            }
        }
    }

    return result_vec;
}

void customFFTWObject::execute_fft_w_zpadd(std::vector<std::complex<double> > &signal, size_t N, bool conjugate)
{

}

std::vector<double> customFFTWObject::fft_correlation_w_ref(std::vector<std::complex<double> > &reference, std::vector<std::complex<double> > &signal)
{
    size_t N = signal.size();
    std::vector<double> result_vec(N);

    int idx_fft = define_fft_w_size(N,false);  // fft definition
    int idx_ifft = define_fft_w_size(N,true);  // ifft definition



    return result_vec;
}

int customFFTWObject::define_fft_w_size(size_t size,bool ifft)
{
    // Check if definition already exists
    bool exists = false;
    int res_index = -1;
    for (int ii = 0; ii < availableSize; ++ii) {
        if(fft_sizes[ii] == size && isIFFT[ii] == ifft){
            exists = true;
            res_index = ii;
        }
    }

    // Create new definition if it does not already exist
    if(exists == false){

        int currIndex = (last_defined+1) % availableSize;
        fft_sizes[currIndex] = size;
        isIFFT[currIndex] = ifft;

        res_index = currIndex;

        fftw_complex *in = getIn(currIndex);
        fftw_complex *out = getOut(currIndex);

        if(defined[currIndex]){
            fftw_free(in);
            fftw_free(out);
            fftw_destroy_plan(p[currIndex]);
        }

        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);

        if(!ifft){
            p[currIndex] = fftw_plan_dft_1d(size,in,out,FFTW_FORWARD,FFTW_ESTIMATE);
        }else{
            p[currIndex] = fftw_plan_dft_1d(size,in,out,FFTW_BACKWARD,FFTW_ESTIMATE);
        }

        setIn(in,currIndex);
        setOut(out,currIndex);

        defined[currIndex] = true;

        ++last_defined;
    }

    return res_index;
}

fftw_complex *customFFTWObject::getIn(int index)
{
    fftw_complex* ret_pntr;
    switch(index){
    case 0:
        ret_pntr = in_1;
        break;
    case 1:
        ret_pntr = in_2;
        break;
    }
    return ret_pntr;
}

fftw_complex *customFFTWObject::getOut(int index)
{
    fftw_complex* ret_pntr;
    switch(index){
    case 0:
        ret_pntr = out_1;
        break;
    case 1:
        ret_pntr = out_2;
        break;
    }
    return ret_pntr;
}

void customFFTWObject::setIn(fftw_complex *in,int index)
{
    switch(index){
    case 0:
        in_1 = in;
        break;
    case 1:
        in_2 = in;
        break;
    }
}

void customFFTWObject::setOut(fftw_complex *out, int index)
{
    switch(index){
    case 0:
        out_1 = out;
        break;
    case 1:
        out_2 = out;
        break;
    }
}
