#include "huffman.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <unordered_map>
#include <fstream>
#pragma once

bool compareNodes(Node* n1, Node* n2) {
    return n1->count() > n2->count();
}

std::string decode(BitField field) {
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

void decode_stream(FILE* ofp, BitField field) {
    int bits = field.bits();
    int index = 0;
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
    }    auto rest = std::vector<bool>();
    for(;index < bits; index++) {
        rest.push_back(field[index]);
    }
    auto iter = rest.begin();
    uint8_t *buffer = new uint8_t[512000];
    int i = 0;
    while(iter != rest.end()) {
        buffer[i] = root->retrieve(&iter);
        if(i == (512000 - 1)) {
            fwrite((void*) buffer, 1, 512000, ofp);
            i = 0;
            continue;
        }
        i++;
    }
    delete[] buffer;
} 

void Load(std::string input, std::string output) {
    FILE* fp = fopen(input.c_str(), "r");
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
    FILE* ofp = fopen(output.c_str(), "w");
    decode_stream(ofp, field);
}

std::vector<Node*> frequency(FILE* fp) {
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

void Save(FILE* ofp, FILE* ifp) {
    auto nodes = frequency(ifp);
    size_t size = nodes.size();
    while(size > 1) {
        std::sort(nodes.begin(), nodes.end(), compareNodes);
        Node* combined = new TreeNode(nodes[size - 1], nodes[size - 2]);
        nodes.pop_back();
        nodes.pop_back();
        nodes.push_back(combined);
        size = nodes.size();
    }
    BitField output_data(150);
    uint8_t first;
    bool first_set = false;
    // Allocate 4 bits for header
    output_data.push_back(false);
    output_data.push_back(false);
    output_data.push_back(false);
    output_data.push_back(false);
    Node* root = nodes[0];
    root->encode_tree(&output_data);
    std::vector<bool> mapper[256];
    auto opath = std::vector<bool>();
    root->map(mapper, opath);
    uint8_t buffer[128];
    while(!feof(ifp)) {
        if(output_data.byte_len() > 0 && !first_set) {
            first = output_data.get_byte(0);
            std::cout << "setting first to " << (int) first << "\n";
            first_set = true;
        }
        if(output_data.byte_len() - 1 > 512000) {
            fwrite((void*) output_data.data(), 1, 511999, ofp);
            output_data.remove_bytes(511999);
        }
        size_t read = fread((void*) buffer, 1, 128, ifp);
        for(int i = 0; i < read; i++) {
            auto cpath = mapper[buffer[i]];
            output_data.extend(cpath);
        }
    }
    fwrite((void*) output_data.data(), 1, output_data.byte_len() + 1, ofp);
    fclose(ifp);
    // Retrieve the first byte in the file to modify the header.
    uint8_t offset = (uint8_t) (output_data.bits() % 8);
    std::cout << (int) offset << "\n";
    BitField f(2);
    f.push_byte(first);
    bool bits[4];
    uint8_t mask = 8;
    int i = 0;
    while(mask > 0) {
        bits[i] = ((offset & mask) != 0);
        std::cout << bits[i] << "\n";
        i++;
        mask >>= 1;
    }
    for(int i = 0; i < 4; i++) {
        if(bits[i]) {
            f.set(i);
            f.print();
        }
    }
    fseek(ofp, SEEK_SET, 0);
    f.print();
    fwrite((void*) f.data(), 1, 1, ofp);
    fclose(ofp);
}