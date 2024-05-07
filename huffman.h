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
            std::vector<bool>* sets, 
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
            std::vector<bool>* sets, 
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
            std::vector<bool>* sets,
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

std::vector<Node*> frequency( FILE* );
std::string decode(BitField field);
bool compareNodes(Node* n1, Node* n2);
void Save(FILE* ofp, FILE* ifp);

#include "huffman.cpp"
#include "CharCount.cpp"
#include "TreeNode.cpp"