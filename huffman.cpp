#include "huffman.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <unordered_map>
#include <fstream>
#pragma once

bool compareCharCount(CharCount x1, CharCount x2) {
    return x1.count() > x2.count();
}

bool compareNodes(Node* n1, Node* n2) {
    return n1->count() > n2->count();
}

void CharCount::encode(BitField* field) {
    field->push_back(true);
    field->push_byte(data);
}

void TreeNode::encode(BitField* field) {
    field->push_back(false);
    left->encode(field);
    right->encode(field);
}

std::vector<Node*> Huffman::frequency(FILE* fp) {
    int counts[256];
    uint8_t buffer[128];
    memset(counts, 0, sizeof(int) * 256);
    while(!feof(fp)) {
        size_t read = fread((void*) buffer, 1, 128, fp);
        for(int i = 0; i < read; i++) {
            counts[buffer[i]]++;
        }
    }
    fseek(fp, SEEK_SET, 0);
    // filter out characters which don't exist and attach the count to the character so they can be sorted. 
    auto pairs = std::vector<Node*>();
    for(int i = 0; i < 256; i++) {
        if(counts[i] != 0) {
            pairs.push_back(new CharCount(counts[i], i));
        }
    }
    return pairs;
}

std::vector<Node*> Huffman::frequency( std::string input ) {
    // Initialize array to 0
    int counts[256];
    memset(counts, 0, sizeof(int) * 256);
    int size = input.size();
    // Iterate through and increment the array using character as the index.
    for(int i = 0; i < size; i++) {
        uint8_t p = (uint8_t) input[i];
        counts[p]++;
    }
    // filter out characters which don't exist and attach the count to the character so they can be sorted. 
    auto pairs = std::vector<Node*>();
    for(int i = 0; i < 256; i++) {
        if(counts[i] != 0) {
            pairs.push_back(new CharCount(counts[i], i));
        }
    }
    return pairs;
}

void CharCount::map(std::unordered_map<uint8_t, std::vector<bool>>* sets, std::vector<bool> path) {
    (*sets)[data] = path;
}


uint8_t TreeNode::retrieve( std::vector<bool>::iterator* begin) {
    // Write the function to end on the end iterator.
    bool val = *(*begin);
    (*begin)++;
    if(val) {
        return right->retrieve(begin);
    }
    return left->retrieve(begin);
}

void TreeNode::map(std::unordered_map<uint8_t, std::vector<bool>>* sets, std::vector<bool> path) {
    auto lpath = std::vector<bool>(path);
    auto rpath = std::vector<bool>(path);
    lpath.push_back(false);
    rpath.push_back(true);
    left->map(sets, lpath);
    right->map(sets, rpath);
}

int TreeNode::count() {
    if(cache != -1) {
        return cache;
    }
    cache = left->count() + right->count();
    return cache;
}

Huffman::~Huffman() {
    delete root;
}

TreeNode::~TreeNode() {
    delete left;
    delete right;
}

BitField Huffman::encode() {
    BitField field = BitField(1);
    root->encode(&field);
    return field;
}

Huffman::Huffman( FILE* fp ) {
    auto nodes = this->frequency(fp);
    size_t size = nodes.size();
    while(size > 1) {
        std::sort(nodes.begin(), nodes.end(), compareNodes);
        Node* combined = new TreeNode(nodes[size - 1], nodes[size - 2]);
        nodes.pop_back();
        nodes.pop_back();
        nodes.push_back(combined);
        size = nodes.size();
    }
    root = nodes[0];
    auto mapper = std::unordered_map<uint8_t, std::vector<bool>>();
    auto opath = std::vector<bool>();
    root->map(&mapper, opath);
    uint8_t buffer[128];
    while(!feof(fp)) {
        size_t read = fread((void*) buffer, 1, 128, fp);
        for(int i = 0; i < read; i++) {
            auto cpath = mapper[buffer[i]];
            std::cout << "mapping " << (int) buffer[i] << "\n";
            opath.insert(opath.end(), cpath.begin(), cpath.end());
        }
    }
    fclose(fp);
    data = opath;
}

Huffman::Huffman( std::string input ) {
    std::vector<Node*> nodes = this->frequency(input);
    size_t size = nodes.size();
    while(size > 1) {
        std::sort(nodes.begin(), nodes.end(), compareNodes);
        Node* combined = new TreeNode(nodes[size - 1], nodes[size - 2]);
        nodes.pop_back();
        nodes.pop_back();
        nodes.push_back(combined);
        size = nodes.size();
    }
    root = nodes[0];
    auto mapper = std::unordered_map<uint8_t, std::vector<bool>>();
    auto opath = std::vector<bool>();
    root->map(&mapper, opath);
    for(char c : input) {
        auto cpath = mapper[c];
        opath.insert(opath.end(), cpath.begin(), cpath.end());
    }
    data = opath;
}

std::string Huffman::decode(BitField field) {
    int bits = field.bits();
    int index = 0;
    std::string decoded;
    uint8_t remaining = field.get_byte(index, 4);
    index += 4;
    bool flag = field[index];
    index++;
    Node* root;
    if(flag) {
        uint8_t byte = field.get_byte(index);
        root = new CharCount(&field, &index);
    }
    else {
        root = new TreeNode(&field, &index);
    }
    int nodes = 0;
    root->count_nodes(&nodes);
    auto rest = std::vector<bool>();
    for(;index < bits; index++) {
        rest.push_back(field[index]);
    }
    auto iter = rest.begin();
    while(iter != rest.end()) {
        decoded += (char) root->retrieve(&iter);
    }
    return decoded;
}

CharCount::CharCount(BitField* field, int* index) {
    data = field->get_byte(*index);
    (*index) += 8;
    weight = 0;
}

TreeNode::TreeNode( BitField* field, int* index) {
    bool flag = (*field)[*index];
    (*index)++;
    if(flag) {
        left = new CharCount(field, index);
    }
    else {
        left = new TreeNode(field, index);
    }
    flag = (*field)[*index];
    (*index)++;
    if(flag) {
        right = new CharCount(field, index);
    }
    else {
        right = new TreeNode(field, index);
    }
}

void CharCount::encode_tree(BitField* field) {
    int bits = field->bits();
    field->push_back(true);
    field->push_byte(data);
}

void TreeNode::encode_tree(BitField* field) {
    field->push_back(false);
    left->encode_tree(field);
    right->encode_tree(field);
}

void Huffman::Save(std::string name) {
    std::ofstream fp(name);
    BitField field(1);
    field.push_back(false);
    field.push_back(false);
    field.push_back(false);
    field.push_back(false);
    root->encode_tree(&field);
    for(bool flag : data) {
        field.push_back(flag);
    }
    uint8_t x = field.bits() % 8;
    uint8_t mask = 1 << 3;
    int i = 0;
    while(mask > 0) {
        bool flag = ((x & mask) != 0);
        if(flag) {
            field.set(i);
        }
        i++;
        mask >>= 1;
    }
    int nodes = 0;
    root->count_nodes(&nodes);
    std::cout << "Encoding " << nodes << " nodes\n";
    fp.write((const char*) field.data(), field.bytes() + 1);
    fp.close();
}

std::string Huffman::Load(std::string name) {
    FILE* fp = fopen(name.c_str(), "r");
    std::vector<uint8_t> data;
    BitField field(1);
    uint8_t buf[8];
    int bytes = 0;
    while(!feof(fp)) {
        int read = fread((void*) buf, 1, 8, fp);
        bytes += read;
        for(int i = 0; i < read; i++) {
            data.push_back(buf[i]);
        }
    }
    fclose(fp);
    uint8_t last = data[data.size() - 1];
    data.pop_back();
    int deb = 1;
    for(uint8_t byte : data) {
        field.push_byte(byte);
        deb++;
    }
    uint8_t remaining = field.get_byte(0, 4);
    uint8_t mask = 128;
    while(remaining > 0) {
        bool flag = ((last & mask) != 0);
        field.push_back(flag);
        mask >>= 1;
        remaining--;
    }
    return Huffman::decode(field);
}

void CharCount::count_nodes(int* x) {
    (*x)++;
}

void TreeNode::count_nodes(int* x) {
    (*x)++;
    left->count_nodes(x);
    right->count_nodes(x);
}