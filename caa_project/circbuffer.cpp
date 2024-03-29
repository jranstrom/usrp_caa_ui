#include "circbuffer.h"

template<class T>
void CircBuffer<T>::push(T pItem)
{
    buffer[m_in++] = pItem;   // Add item to internal buffer
    ++mk_in;
    // Internal buffer full
    if(m_in >= capacity){
        m_in = 0;
    }

    overflow = this->check_overflow();
}circ_pop

template<class T>
void CircBuffer<T>::reset_buffer()
{
    m_in    = 0;
    m_out   = 0;
    mk_in   = 0;
    mk_out  = 0;
}

template<class T>
void CircBuffer<T>::reset_pop()
{
    if(mk_in <= capacity){
        m_out   = 0;
        mk_out  = 0;
    }else{
        throw std::runtime_error("[ERROR] In circ_buffer::reset_pop() - Reset pop cannot be performed on over-filled buffer");
    }
}

template<class T>
size_t CircBuffer<T>::pop(T *pItem)
{
    if (mk_out+1 > mk_in){    // will potentially take out more than is put in
        return 0;             // don't do it!
    } else {
        *pItem = buffer[m_out++];
        if(m_out >= capacity)
            m_out = 0;
        ++mk_out;
        return mk_in-mk_out+1; // Return difference
    }
}

template<class T>
bool CircBuffer<T>::back(T *out_p, int offset)
{
    if (offset < capacity && ((int)mk_in - offset)>=0){

        int m_m = (int)m_in-offset;

        if(m_m < 0){
            m_m = capacity-m_m;
        }
        *out_p = buffer[m_m];

        return true;
    }else{
        return false;
    }
}

template<class T>
size_t CircBuffer<T>::trim_buffer(bool resetRead, bool allowFilled)
{
    if (this->check_filled() and not allowFilled){
        return 0;
    }
    capacity = m_in;
    m_in = 0;
    if(resetRead){
        mk_out = 0;
        m_out = 0;
    }
    return capacity;
}

template<class T>
size_t CircBuffer<T>::circ_pop(T *pItem)
{
    *pItem = buffer[m_out++];
    if(m_out >= capacity)
        m_out = 0;
    ++mk_out;
    return mk_in-mk_out+1; // Return difference
}

template<class T>
bool CircBuffer<T>::get_at(T *pItem, size_t mi)
{
    if(mi < capacity){
        *pItem = buffer[mi];
        return true;
    }
    return false;
}

template<class T>
std::vector<T> CircBuffer<T>::extract_range(size_t strt_mk_i, size_t L)
{
    std::vector<T> res_v;
    if (strt_mk_i + L <= mk_in){
        if(mk_in < capacity || strt_mk_i > mk_in-capacity){
            for(size_t i=0;i<L;i++){
                int ik = (i + strt_mk_i) % capacity;
                res_v.push_back(buffer[ik]);
            }
        }else{
            //std::cout << "Error: " << strt_mk_i << " : " << mk_in << " : " << capacity << std::endl;
            throw std::runtime_error("Trying to access overwritten element");
        }
    }else{
        //throw std::runtime_error("Trying to access entry outside range");
    }
    return res_v;
}

template<class T>
std::vector<T> CircBuffer<T>::to_vector()
{
    std::vector<T> ret_vec;

    if(not this->check_filled()){

        for(int ii=0;ii<m_in;ii++){
            ret_vec.push_back(buffer[ii]);
        }
    }
    return ret_vec;
}

template<class T>
size_t CircBuffer<T>::skip(size_t num_skip)
{
    size_t mk_out_tmp = std::min(mk_out+num_skip,mk_in); // skip desired, or to end
    if(num_skip == 0){
        mk_out_tmp = mk_in; // skip to end
    }
    num_skip = mk_out_tmp-mk_out;                       // calculate actual skip
    mk_out += num_skip;                                 // apply skip to global index
    m_out = (m_out+num_skip) % capacity;                // apply skip to circular index

    return mk_out;
}
