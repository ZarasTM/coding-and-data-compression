#ifndef KDDLIST2_BITCHAR_H
#define KDDLIST2_BITCHAR_H
#include <string>

class BitChar {
public:
    char* c;
    int shift = 0;
    std::string bits;

    BitChar();

    void setBits(std::string data);

    int insertBits(std::ofstream& outf);

    void writeBits(std::ofstream& outf);

    std::string getBits(char x);

    ~BitChar();
};


#endif //KDDLIST2_BITCHAR_H
