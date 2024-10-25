#include "bitfield.h"

void BitField::set(int pos) {
    int byte = pos / 8;
    int bit = pos % 8;
    uint8_t mask = 128 >> bit;
    dat[byte] |= mask;
}

void BitField::unset(int pos) {
    int byte = pos / 8;
    int bit = pos % 8;
    uint8_t mask = 128 >> bit;
    mask = ~mask;
    dat[byte] &= mask;
}

uint8_t BitField::get_byte(int pos, int n = 8) {
    if(n == 8 && (pos % 8) == 0) {
        return dat[pos / 8];
    } 
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

size_t BitField::bits() {
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
        reserve(byte_size * 2);
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

size_t BitField::byte_len() {
    return byte_length;
}

size_t BitField::size() {
    return byte_size;
}

void BitField::extend(BitField field) {
    size_t bits = field.bits();
    int i = 0;
    while(bits <= 8) {
        uint8_t byte = field.get_byte(i);
        this->push_byte(byte);
        i++;
        bits -= 8;
    }
    for(int i = 0; i < bits; i++) {
        this->push_back(field[i]);
    }
}

void BitField::extend(std::vector<bool> field) {
    for(bool b : field) {
        this->push_back(b);
    }
}

void BitField::remove_bytes(size_t count) {
    if(count >= byte_length + 1) {
        delete dat;
        byte_length = 0;
        bit_length = 0;
        memset(dat, 0, byte_size);
        return;
    }
    // TODO Finish this.
    byte_length -= count;
    memcpy((void*) dat, (void*) (dat + count), byte_length + 1);
    memset((void*) (dat + (byte_length + 1)), 0, count);
}

BitField::BitField( std::vector<bool> flags ) {
    size_t byte_size = (flags.size() / 8) + 1;
    dat = new uint8_t[byte_size];
    for(bool b : flags) {
        this->push_back(b);
    }
}