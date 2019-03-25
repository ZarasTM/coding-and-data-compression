#ifndef KODROZNICOWE_TGALOADER_H
#define KODROZNICOWE_TGALOADER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct Pixel{
    short R;
    short G;
    short B;
};

class TgaOperator {
public:
    vector<Pixel*> rawImg;
    char* fileBuffer;

    int height;
    int width;

    void loadImg(string path);
    vector<Pixel*> getRawImg();

    TgaOperator(string path);
};


#endif //KODROZNICOWE_TGALOADER_H
