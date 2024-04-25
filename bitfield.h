#include <iterator>
#include <string.h>
#include <iostream>
#include <fstream>
#pragma once

class BitField {
    public:
        BitField( size_t size );
        void write( std::ofstream );
        bool operator[]( size_t );
        uint8_t* data();
        int size();
        int bytes();
        int bits();
        void set(int);
        void print();
        void push_back( bool );
        void push_byte( uint8_t );
        uint8_t get_byte(int pos, int n);
        // void push_front( bool );
        void reserve( size_t );
    private:
        int byte_size = 1;
        int byte_length = 0;
        int bit_length = 0;
        uint8_t* dat;
};

void BitField::set(int pos) {
    int byte = pos / 8;
    int bit = pos % 8;
    uint8_t mask = 128 >> bit;
    dat[byte] |= mask;
}

uint8_t BitField::get_byte(int pos, int n = 8) {
    uint8_t val = 0;
    uint8_t mask = 1 << (n - 1);
    while(mask > 0) {
        if((*this)[pos]) {
            val |= mask;
        }
        mask >>= 1;
        pos++;
    }
    return val;
}

int BitField::bits() {
    return ((byte_size - 1) * 8) + bit_length;
}

bool BitField::operator[](size_t index) {
    int byte = index / 8;
    int bit = index % 8;
    uint8_t mask = 128 >> bit;
    return ((dat[byte] & mask) != 0);
}

void BitField::push_byte(uint8_t byte) {
    for(uint8_t mask = 128; mask > 0; mask >>= 1) {
        push_back(((byte & mask) != 0));
    }
}

void BitField::write(std::ofstream fp) {
    fp.write((const char*) dat, byte_length);
}

void BitField::push_back(bool flag) {
    if(flag) {
        uint8_t mask = 128 >> bit_length;
        dat[byte_length] |= mask;
    }
    bit_length++;
    byte_length += bit_length / 8;
    bit_length %= 8;
    if(byte_length == byte_size) {
        reserve(byte_size + 1);
    }
}

void BitField::print() {
    for(int i = 0; i < byte_size; i++) {
        for(uint8_t mask = 128; mask > 0; mask >>= 1) {
            std::cout << ((dat[i] & mask) != 0);
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}


void BitField::reserve(size_t size) {
    if(size <= byte_size) {
        return;
    }
    uint8_t* old = dat;
    dat = new uint8_t[size];
    dat[size - 1] = 0;
    memcpy(dat, old, byte_size);
    delete[] old;
    byte_size = size;
}

BitField::BitField(size_t size) {
    byte_size = size;
    dat = new uint8_t[size];
    memset(dat, 0, byte_size);
}

uint8_t* BitField::data() {
    return dat;
}

int BitField::bytes() {
    return byte_length;
}

int BitField::size() {
    return byte_size;
}
