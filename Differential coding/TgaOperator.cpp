#include "TgaOperator.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

void TgaOperator::loadImg(string path){
    ifstream inFile(path, ios::binary | ios_base::in);
    inFile >> noskipws;

    if(!inFile.is_open()){
        fprintf(stderr, "ERROR: Cannot read from file");
        exit(1);
    }

    auto * temp = static_cast<unsigned short *>(malloc(sizeof(unsigned short)));

    inFile.read(fileBuffer, 18);

    // Getting width 8+4'th byte
    inFile.seekg(12);
    inFile.read(reinterpret_cast<char *>(temp), sizeof(unsigned short));
    this->width = *temp;

    // Getting height (next two bytes)
    inFile.read(reinterpret_cast<char *>(temp), sizeof(unsigned short));
    this->height = *temp;
    free(temp);

    // Load img to oldImg
    inFile.seekg(18+1);
    Pixel* pixel;
    auto* col = static_cast<char *>(malloc(sizeof(char)));

    for(int i=0; i<width*height; i++){
        pixel = new Pixel;

        inFile.read(col, sizeof(unsigned char));
        pixel->B = (unsigned char)(*col);
        inFile.read(col, sizeof(unsigned char));
        pixel->R = (unsigned char)(*col);
        inFile.read(col, sizeof(unsigned char));
        pixel->G = (unsigned char)(*col);

        rawImg.push_back(pixel);
    }

    // Place in buffer
    inFile.read(fileBuffer + 18, 26);

    inFile.close();

    cout << "Loaded " << rawImg.size() << " pixels" << endl;
    cout << "Image dimentions are " << width << " x " << height << endl;
}

vector<Pixel*> TgaOperator::getRawImg(){
    return rawImg;
}

TgaOperator::TgaOperator(string path){
    this->fileBuffer = new char[18+26];
    loadImg(path);
}