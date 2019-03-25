#include <fstream>
#include <sstream>
#include <bitset>
#include <iostream>
#include "BitChar.h"

using namespace std;

BitChar::BitChar()
{
    // Set shift to 0, set size of c to 8 bits and init it with 0
    shift = 0;
    c = (char*)calloc(1, sizeof(char));
}

void BitChar::setBits(string data)
{
    bits = data;
}

int BitChar::insertBits(ofstream& outf)
{
    int total = 0;

    for(auto &bit : bits)
    {
        // We shift our c left by 1
        *c = *c << 1;
        shift++;
        total++;
        // If bit is 1 we add it at the end of c
        if(bit == '1') *c |= 1;

        if(shift == 8)
        {
            writeBits(outf);
            shift = 0;
            free(c);
            c = (char*)calloc(1, sizeof(char));
        }
    }

    // Push any remaining bits
    if(shift > 0)
    {
        *c <<= (8 - shift);
        writeBits(outf);
        free(c);
        c = (char*)calloc(1, sizeof(char));
    }

    return total;
}

void BitChar::writeBits(ofstream& outf)
{
    outf << *c;
}

//Outputs a char in binary format
string BitChar::getBits(char x)
{
    return bitset<8>(x).to_string();
}

BitChar::~BitChar()
{
    if(c) free(c);
}