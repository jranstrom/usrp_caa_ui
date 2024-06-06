#ifndef CIRCBUFFER_H
#define CIRCBUFFER_H

#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <typeinfo>
#include <iostream>

template <class T>
class CircBuffer {
public:

    CircBuffer(size_t buffer_cap=1e8) : m_in(0), m_out(0), mk_in(0), mk_out(0) {
        capacity = buffer_cap;
        if(buffer_cap >0){
            buffer = new T[capacity];
            allocatedMemory = true;
        }
    }

    template <typename U = T>
    CircBuffer(const CircBuffer<U> & rhs_cbuff){

        capacity = rhs_cbuff.get_capacity();
        buffer = new T[capacity];
        allocatedMemory = true;

        mk_in = rhs_cbuff.get_push_count();
        mk_out = rhs_cbuff.get_pop_count();

        m_in = capacity % mk_in;
        m_out = capacity % mk_out;

        //std::cout << rhs_cbuff.get_capacity() << std::endl;

        T c_entry;
        for (size_t i=0;i<capacity;i++){
            if(rhs_cbuff.get_at(c_entry,i))
                buffer[i] = c_entry;
        }
    }

    template <typename U = T>
    CircBuffer(const std::vector<U> & vec){
        m_in = vec.size()-1;
        mk_in = vec.size();
        m_out = 0;
        mk_out = 0;

        capacity = vec.size();

        if(buffer != nullptr && allocatedMemory){
            delete[]buffer;
            allocatedMemory = false;
        }
        buffer = new T[capacity];
        allocatedMemory = true;

        for (size_t i=0;i<capacity;i++){
            buffer[i] = vec[i];
        }
    }

    ~CircBuffer(){
        if(buffer != nullptr && allocatedMemory){
            delete[]buffer;
        }
    }

    void push(T pItem){

        buffer[m_in++] = pItem;   // Add item to internal buffer
        ++mk_in;
        // Internal buffer full
        if(m_in >= capacity){
            m_in = 0;
        }

        overflow = this->check_overflow();
    }

    void reset_buffer(){
        m_in    = 0;
        m_out   = 0;
        mk_in   = 0;
        mk_out  = 0;
    }

    void reset_pop(){
        if(mk_in <= capacity){
            m_out   = 0;
            mk_out  = 0;
        }else{
            throw std::runtime_error("[ERROR] In circ_buffer::reset_pop() - Reset pop cannot be performed on over-filled buffer");
        }
    }

    size_t pop(T* pItem){
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

    size_t pop_w_count(T* pItem){
        if (mk_out+1 > mk_in){    // will potentially take out more than is put in
            return 0;             // don't do it!
        } else {
            *pItem = buffer[m_out++];
            if(m_out >= capacity)
                m_out = 0;
            ++mk_out;
            return mk_out; // Return difference
        }
    }

    bool back(T* out_p,int offset=0){

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

    size_t trim_buffer(bool resetRead = false,bool allowFilled = false){
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

    size_t circ_pop(T* pItem){
        *pItem = buffer[m_out++];
        if(m_out >= capacity)
            m_out = 0;
        ++mk_out;
        return mk_in-mk_out+1; // Return difference
    }

    bool get_at(T* pItem, size_t mi){
        if(mi < capacity){
            *pItem = buffer[mi];
            return true;
        }
        return false;
    }

    std::vector<T> extract_range(size_t strt_mk_i,size_t L){
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

    int get_internal_buff_index(size_t strt_mk_i){
        return (strt_mk_i % capacity);
    }

    std::vector<T> to_vector(){
        std::vector<T> out_vec;

        if(not this->check_filled()){

            for(int ii=0;ii<m_in;ii++){
                out_vec.push_back(buffer[ii]);
            }
        }
        return out_vec;
    }

    template <typename U = T>
    void from_vector(const std::vector<U> & in_vec){
        m_in = in_vec.size()-1;
        mk_in = in_vec.size();
        m_out = 0;
        mk_out = 0;

        capacity = in_vec.size();

        if(buffer != nullptr && allocatedMemory){
            delete[]buffer;
            allocatedMemory = false;
        }

        buffer = new T[capacity];
        allocatedMemory = true;

        for (size_t i=0;i<capacity;i++){
            buffer[i] = in_vec[i];
        }
    }

    size_t skip(size_t num_skip = 1){
        size_t mk_out_tmp = std::min(mk_out+num_skip,mk_in); // skip desired, or to end
        if(num_skip == 0){
            mk_out_tmp = mk_in; // skip to end
        }
        num_skip = mk_out_tmp-mk_out;                       // calculate actual skip
        mk_out += num_skip;                                 // apply skip to global index
        m_out = (m_out+num_skip) % capacity;                // apply skip to circular index

        return mk_out;
    }

    bool check_overflow(){
        return (((int)mk_out-(int)mk_in) > (int)capacity);
    }

    bool check_filled(){
        return (mk_in >= capacity);
    }

    size_t get_push_count(){
        return mk_in;
    }

    size_t get_pop_count(){
        return mk_out;
    }

    size_t get_internal_pop_count(){
        return m_out;
    }

    bool check_any_overflow(){
        return overflow;
    }

    size_t get_capacity(){
        return capacity;
    }

private:
    //circ_buffer(const circ_buffer&);              // Disabled copy constructor
    //circ_buffer& operator = (const circ_buffer&); // Disabled assign operator


    T *buffer;
    bool overflow = false;
    bool underflow = true;
    bool allocatedMemory = false;
    size_t m_in;    // holds circular 'in' index
    size_t m_out;   // holds circular 'out' index

    size_t nc_out = 0;  // holds non-circular index 'out'

    size_t mk_in;   // holds absolute 'in' index
    size_t mk_out;  // holds absolute 'out' index

    size_t capacity;

};

#endif // CIRCBUFFER_H
