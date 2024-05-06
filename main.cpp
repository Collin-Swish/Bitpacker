#include <iostream>
#include "huffman.h"
#include <fstream>
#include <string>
#include "bitfield.h"
#include <sstream>

using namespace std;

string lower(string x) {
    int size1 = x.size();
    for(int i = 0; i < size1; i++) {
        x[i] = tolower(x[i]);
    }
    return x;
}

void Compress(string in_file, string out_file) {
    FILE* fp = fopen(in_file.c_str(), "r");
    Huffman tree(fp);
    cout << "Saving\n";
    tree.Save(out_file);
}

void Compress2(string in_file, string out_file) {
    FILE* ifp = fopen(in_file.c_str(), "r");
    FILE* ofp = fopen(out_file.c_str(), "w");
    Save(ofp, ifp);
}

void Decompress(string in_file, string out_file) {
    string data = Huffman::Load(in_file);
    ofstream fp(out_file);
    fp.write(data.c_str(), data.size());
}

int main(int argc, char* argv[]) {
    string arg1 = string(argv[1]);
    string arg2 = string(argv[2]);
    string arg3 = string(argv[3]);
    arg1 = lower(arg1);
    if(arg1 == "compress" || arg1 == "-c") {
        Compress2(arg2, arg3);
    }
    else if(arg1 == "decompress" || arg1 == "-d") {
        Decompress(arg2, arg3);
    }

}