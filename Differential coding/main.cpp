#include <iostream>
#include "TgaOperator.h"
#include <cmath>

struct PixelSig{
    short R;
    short G;
    short B;
};

int bitsByCol = 7;
vector<PixelSig*> encImg;
vector<Pixel*> decImg;
vector<Pixel*> rawImg;

double mseAll = 0;
double mseR = 0;
double mseG = 0;
double mseB = 0;

short maxNum = 255-pow(2, 8-bitsByCol);

void quantize(){
    for(auto &i : encImg) {
        // For Red
        i->R = i->R >> (8 - bitsByCol - 1);
        i->R = i->R | 1;
        i->R = i->R << (8 - bitsByCol - 1);

        // For Green
        i->G = i->G >> (8 - bitsByCol - 1);
        i->G = i->G | 1;
        i->G = i->G << (8 - bitsByCol - 1);

        // For Blue
        i->B = i->B >> (8 - bitsByCol - 1);
        i->B = i->B | 1;
        i->B = i->B << (8 - bitsByCol - 1);
    }
}

void encode(vector<Pixel*> img){
    short difR = 0;
    short difG = 0;
    short difB = 0;
    PixelSig* newPixel;

    for(auto &i : img) {
        newPixel = new PixelSig;

        difR = i->R - difR;
        difG = i->G - difG;
        difB = i->B - difB;

        newPixel->R = difR;
        newPixel->G = difG;
        newPixel->B = difB;

        encImg.push_back(newPixel);
    }

    quantize();
}

void decode(){
    short difR = 0;
    short difG = 0;
    short difB = 0;
    Pixel* newPixel;

    newPixel = new Pixel;
    newPixel->R = encImg.at(0)->R;
    newPixel->G = encImg.at(0)->G;
    newPixel->B = encImg.at(0)->B;

    decImg.push_back(newPixel);

    for(int i=1; i<encImg.size(); i++) {
        newPixel = new Pixel;

        difR = encImg.at(i)->R + encImg.at(i-1)->R;
        if(difR>maxNum) difR = maxNum;
        difG = encImg.at(i)->G + encImg.at(i-1)->G;
        if(difG>maxNum) difG = maxNum;
        difB = encImg.at(i)->B + encImg.at(i-1)->B;
        if(difB>maxNum) difB = maxNum;

        newPixel->R = difR;
        newPixel->G = difG;
        newPixel->B = difB;

        decImg.push_back(newPixel);
    }
}

void saveImg(string path, char* fileBuffer, vector<PixelSig*> img){
    ofstream outFile(path);
    outFile << noskipws;

    if(!outFile.is_open()){
        fprintf(stderr, "ERROR: Cannot write to file");
        exit(1);
    }

    // Get header from buffer
    int buffIter = 0;
    while(buffIter < 18){
        outFile << fileBuffer[buffIter];
        buffIter++;
    }

    // Get color data
    for (auto &i : img) {
        outFile << i->G;
        outFile << i->B;
        outFile << i->R;
    }

    // Get footer from buffer
    while(buffIter < (18 + 26)){
        outFile << fileBuffer[buffIter];
        buffIter++;
    }

    outFile.close();
}

void saveImg(string path, char* fileBuffer, vector<Pixel*> img){
    ofstream outFile(path);
    outFile << noskipws;

    if(!outFile.is_open()){
        fprintf(stderr, "ERROR: Cannot write to file");
        exit(1);
    }

    // Get header from buffer
    int buffIter = 0;
    while(buffIter < 18){
        outFile << fileBuffer[buffIter];
        buffIter++;
    }

    // Get color data
    for (auto &i : img) {
        outFile << (unsigned char)i->G;
        outFile << (unsigned char)i->B;
        outFile << (unsigned char)i->R;
    }

    // Get footer from buffer
    while(buffIter < (18 + 26)){
        outFile << fileBuffer[buffIter];
        buffIter++;
    }

    outFile.close();
}

void MSE(){
    if(rawImg.size() != decImg.size()){
        cout << "Vectors have different sizes RAW: " << rawImg.size() << ", DEC: " << decImg.size() << endl;
        return;
    }

    for(int i=0; i<rawImg.size(); i++){
        mseR = mseR + pow(rawImg.at(i)->R - decImg.at(i)->R, 2);
        mseG = mseG + pow(rawImg.at(i)->G - decImg.at(i)->G, 2);
        mseB = mseB + pow(rawImg.at(i)->B - decImg.at(i)->B, 2);
    }

    mseAll = (mseR + mseG + mseB)/(rawImg.size()*3);
    mseR = mseR/rawImg.size();
    mseG = mseG/rawImg.size();
    mseB = mseB/rawImg.size();

    cout << "-=======-MSE-======-\nAll:\t" << mseAll << "\nR:\t" << mseR << "\nG:\t" << mseG << "\nB:\t" << mseB << endl;
}

void SNR(){
    double snrAll = 0;
    double snrR = 0;
    double snrG = 0;
    double snrB = 0;

    for(int i=0; i<rawImg.size(); i++){
        snrR = snrR + pow(rawImg.at(i)->R, 2);
        snrG = snrG + pow(rawImg.at(i)->G, 2);
        snrB = snrB + pow(rawImg.at(i)->B, 2);
    }

    snrAll = (snrR + snrG + snrB)/(rawImg.size()*3*mseAll);
    snrR = snrR/(rawImg.size()*mseR);
    snrG = snrG/(rawImg.size()*mseG);
    snrB = snrB/(rawImg.size()*mseB);

    snrAll = 10*log(snrAll);
    snrR = 10*log(snrR);
    snrG = 10*log(snrG);
    snrB = 10*log(snrB);

    cout << "\n-=======-SNR-======-\nAll:\t" << snrAll << " dB\nR:\t" << snrR << " dB\nG:\t" << snrG << " dB\nB:\t" << snrB << " dB" << endl;
}

int main(int argc, char** argv) {
    if(argc>1){
        char* c = argv[1];
        bitsByCol = atoi(c);
        if(bitsByCol>7){
            cout << "Reasigning number of bits from " << bitsByCol << " to 7" << endl;
            bitsByCol = 7;
        }
    }
    cout << "Preparing img for encoding..." << endl;
    TgaOperator* loader = new TgaOperator("/root/CLionProjects/KodRoznicowe/testy4/example0.tga");
    rawImg = loader->getRawImg();
    encode(rawImg);
    decode();
    //saveImg("/root/CLionProjects/KodRoznicowe/exampleEnc.tga", loader->fileBuffer, encImg);
    saveImg("/root/CLionProjects/KodRoznicowe/exampleDec.tga", loader->fileBuffer, decImg);
    MSE();
    SNR();
    cout << "Closing program..." << endl;
    return 0;
}