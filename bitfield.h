#include <iterator>
#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>
#pragma once

class BitField {
    public:
        BitField( size_t size );
        void write( std::ofstream );
        bool operator[]( size_t );
        uint8_t* data();
        // Number of bytes allocated
        size_t size();
        // The length of bytes in the list.
        size_t byte_len();
        // The total number of bits.
        size_t bits();
        // Sets the bit at this index to true. 
        void set(int);
        // Sets the bit at this index to be false.
        void unset(int);
        void print();
        void push_back( bool );
        void push_byte( uint8_t );
        void extend( BitField );
        void extend( std::vector<bool> );
        void remove_bytes( size_t );
        uint8_t get_byte(int pos, int n);
        void reserve( size_t );
    private:
        // Number of bytes allocated
        size_t byte_size = 1;
        size_t byte_length = 0;
        uint8_t bit_length = 0;
        uint8_t* dat;
};

#include "bitfield.cpp"