#include <iostream>
#include <vector>
#include <unordered_map>
#include "bitfield.h"
#pragma once

class Node {
    public:
        virtual void count_nodes(int*) {}
        virtual int count() {return 0;}
        virtual void encode( BitField* ) {}
        virtual void encode_tree( BitField* ) {}
        virtual uint8_t retrieve( std::vector<bool>::iterator* begin ) {
            std::cout << "This function should never run\n";
            return 0;
        };
        virtual void map(
            std::unordered_map<uint8_t, std::vector<bool>>* sets, 
            std::vector<bool> path
        ) {}
};

class CharCount : public Node {
    public:
        void encode( BitField* );
        void count_nodes(int*);
        void encode_tree( BitField* );
        int count() {return weight;};
        void map(
            std::unordered_map<uint8_t, std::vector<bool>>* sets, 
            std::vector<bool> path
        );
        uint8_t retrieve( std::vector<bool>::iterator* begin ) {return data;};
        CharCount( BitField*, int* );
        CharCount( int count, uint8_t data ) {
            this->weight = count;
            this->data = data;
        }
        int weight;
        uint8_t data;
};

class TreeNode : public Node {
    public:
        void count_nodes(int*);
        void encode( BitField* );
        void encode_tree( BitField* );
        TreeNode(Node* left, Node* right) {
            this->left = left;
            this->right = right;
        }
        void map(
            std::unordered_map<uint8_t, std::vector<bool>>* sets,
            std::vector<bool> path
        );
        int count();
        uint8_t retrieve( std::vector<bool>::iterator* begin );
        TreeNode();
        TreeNode( BitField* , int* );
        ~TreeNode();
        Node* left = NULL;
        Node* right = NULL;
    private:
        int cache = -1;
};

class Huffman {
    public:
        Huffman( std::string );
        Huffman( FILE* );
        ~Huffman();
        void Save( std::string );
        static std::string Load( std::string );
        BitField encode();
        static std::string decode( BitField );
    private:
        std::vector<Node*> frequency( std::string );
        std::vector<Node*> frequency( FILE* );
        std::vector<bool> data;
        Node* root = NULL;
        
        
};

bool compareNodes(Node* n1, Node* n2);

#include "huffman.cpp"
#include "CharCount.cpp"
#include "TreeNode.cpp"