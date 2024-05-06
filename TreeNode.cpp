#include "huffman.h"

void TreeNode::encode(BitField* field) {
    field->push_back(false);
    left->encode(field);
    right->encode(field);
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

TreeNode::~TreeNode() {
    delete left;
    delete right;
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

void TreeNode::encode_tree(BitField* field) {
    field->push_back(false);
    left->encode_tree(field);
    right->encode_tree(field);
}

void TreeNode::count_nodes(int* x) {
    (*x)++;
    left->count_nodes(x);
    right->count_nodes(x);
}