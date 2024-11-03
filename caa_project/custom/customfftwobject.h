#ifndef CUSTOMFFTWOBJECT_H
#define CUSTOMFFTWOBJECT_H

#include <fftw3.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <numbers>
#include <vector>
#include <complex>

class customFFTWObject
{
public:
    customFFTWObject();
    ~customFFTWObject();

    std::vector<std::complex<double>> fft_w_zpadd(std::vector<std::complex<double>> &signal,size_t N,bool conjugate);
    void execute_fft_w_zpadd(std::vector<std::complex<double>> &signal,size_t N,bool conjugate);
    std::vector<double> fft_correlation_w_ref(std::vector<std::complex<double> > &reference, std::vector<std::complex<double> > &signal);

    int define_fft_w_size(size_t size, bool ifft);

private:

    fftw_complex *in_1;
    fftw_complex *out_1;

    fftw_complex *in_2;
    fftw_complex *out_2;

    static const size_t availableSize = 2;
    size_t fft_sizes[availableSize];
    bool defined[availableSize];
    bool isIFFT[availableSize];
    fftw_plan p[availableSize];

    int last_defined = -1;

    fftw_complex* getIn(int index);
    fftw_complex* getOut(int index);

    void setIn(fftw_complex * in,int index);
    void setOut(fftw_complex * out,int index);


};



#endif // CUSTOMFFTWOBJECT_H
