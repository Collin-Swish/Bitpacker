#include "huffman.h"

void CharCount::encode(BitField* field) {
    field->push_back(true);
    field->push_byte(data);
}

void CharCount::map(std::unordered_map<uint8_t, std::vector<bool>>* sets, std::vector<bool> path) {
    (*sets)[data] = path;
}

CharCount::CharCount(BitField* field, int* index) {
    data = field->get_byte(*index);
    (*index) += 8;
    weight = 0;
}

void CharCount::encode_tree(BitField* field) {
    int bits = field->bits();
    field->push_back(true);
    field->push_byte(data);
}

void CharCount::count_nodes(int* x) {
    (*x)++;
}
