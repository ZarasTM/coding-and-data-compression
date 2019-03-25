#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#define HEADER_SIZE 18

using namespace std;

struct PIXEL{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

struct PIXEL_SIG{
    char R;
    char G;
    char B;
};

struct PROB{
    char code;
    int count;
};

int option = 1; // Option is for selecting prediction algorythm 1-7 (old ones) 8 (new one)
unsigned short width;
unsigned short height;
vector<PROB> vProb;
vector<PROB> vProbR;
vector<PROB> vProbG;
vector<PROB> vProbB;

char newStandard(unsigned char N, unsigned char W) {
    if(N*W >= max(W, N)) {
        return max(W, N);
    } else if(N*W <= min(W, N)) {
        return min(W, N);
    } else {
        return W + N - N*W;
    }
}

bool resolveSymbol(vector<PROB> *v,char c){
    for(unsigned long i=0; i<v->size(); i++){
        if(v->at(i).code == c){
            v->at(i).count++;
            return true;
        }
    }
    PROB newTemp;
        newTemp.code = c;
        newTemp.count = 1;
    v->push_back(newTemp);
    return false;
}

int main(int argc, char* argv[]) {
    char* c = argv[1];
    option = atoi(c);
    string path = "/root/CLionProjects/JPEG-LS/tests/example3.tga";
    ifstream inFile(path, ios::binary | ios_base::in);
    inFile >> noskipws;

    if(!inFile.is_open()){
        fprintf(stderr, "ERROR: Cannot open file");
        return 0;
    }

    auto * temp = static_cast<unsigned short *>(malloc(sizeof(unsigned short)));;

    // Getting width 8+4'th byte
    inFile.seekg(12);
    inFile.read(reinterpret_cast<char *>(temp), sizeof(unsigned short));
    width = *temp;

    // Getting height (next two bytes)
    inFile.read(reinterpret_cast<char *>(temp), sizeof(unsigned short));
    height = *temp;
    free(temp);

    // Read pixel data
    PIXEL img[width+1][height+1];

    char *tmp = static_cast<char *>(malloc(sizeof(char)));
    inFile.seekg(HEADER_SIZE+1);

    for(int i = 0; i <= width; i++) {
        for(int j = 0; j<= height; j++) {
            if(i != 0 && j != 0) {
                inFile.read(tmp, sizeof(unsigned char));
                img[i][j].R = static_cast<unsigned char>(*tmp);

                inFile.read(tmp, sizeof(unsigned char));
                img[i][j].G = static_cast<unsigned char>(*tmp);

                inFile.read(tmp, sizeof(unsigned char));
                img[i][j].B = static_cast<unsigned char>(*tmp);
            }else{
                img[i][j].R = 0;
                img[i][j].G = 0;
                img[i][j].B = 0;
            }
        }
    }

    inFile.close();

    PIXEL_SIG newImg[width][height];
    PIXEL x, N, W, NW;

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            x = img[i+1][j+1];
            switch(option){
                case 1: // X = W
                    W = img[i][j+1];
                    newImg[i][j].R = x.R - W.R;
                    newImg[i][j].G = x.G - W.G;
                    newImg[i][j].B = x.B - W.B;
                    break;
                case 2: // X = N
                    N = img[i+1][j];
                    newImg[i][j].R = x.R - N.R;
                    newImg[i][j].G = x.G - N.G;
                    newImg[i][j].B = x.B - N.B;
                    break;
                case 3: // X = NW
                    NW = img[i][i];
                    newImg[i][j].R = x.R - (NW.R);
                    newImg[i][j].G = x.G - (NW.G);
                    newImg[i][j].B = x.B - (NW.B);
                    break;
                case 4: // X = N + W - NW
                    N = img[i+1][j];
                    W = img[i][j+1];
                    NW = img[i][i];
                    newImg[i][j].R = x.R - (N.R + W.R - (NW.R));
                    newImg[i][j].G = x.G - (N.G + W.G - (NW.G));
                    newImg[i][j].B = x.B - (N.B + W.B - (NW.B));
                    break;
                case 5: // X = N + (W - NW)/2
                    N = img[i+1][j];
                    W = img[i][j+1];
                    NW = img[i][i];
                    newImg[i][j].R = x.R - (N.R + (W.R - (NW.R))/(char)2);
                    newImg[i][j].G = x.G - (N.G + (W.G - (NW.G))/(char)2);
                    newImg[i][j].B = x.B - (N.B + (W.B - (NW.B))/(char)2);
                    break;
                case 6: // X = W + (N - NW)/2
                    N = img[i+1][j];
                    W = img[i][j+1];
                    NW = img[i][i];
                    newImg[i][j].R = x.R - (W.R + (N.R - (NW.R))/(char)2);
                    newImg[i][j].G = x.G - (W.G + (N.G - (NW.G))/(char)2);
                    newImg[i][j].B = x.B - (W.B + (N.B - (NW.B))/(char)2);
                    break;
                case 7: // X = (N + W)/2
                    N = img[i+1][j];
                    W = img[i][j+1];
                    newImg[i][j].R = x.R - (N.R + W.R)/(char)2;
                    newImg[i][j].G = x.G - (N.G + W.G)/(char)2;
                    newImg[i][j].B = x.B - (N.B + W.B)/(char)2;
                    break;
                case 8: // New standard
                    N = img[i+1][j];
                    W = img[i][j+1];
                    newImg[i][j].R = x.R - newStandard(N.R, W.R);
                    newImg[i][j].G = x.G - newStandard(N.G, W.G);
                    newImg[i][j].B = x.B - newStandard(N.B, W.B);
                    break;
            }
        }
    }

    // Get count of elements
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            // For all
            resolveSymbol(&vProb, newImg[i][j].R);
            resolveSymbol(&vProb, newImg[i][j].G);
            resolveSymbol(&vProb, newImg[i][j].B);

            // For R
            resolveSymbol(&vProbR, newImg[i][j].R);

            // For G
            resolveSymbol(&vProbG, newImg[i][j].G);

            // For B
            resolveSymbol(&vProbB, newImg[i][j].B);
        }
    }

    long double entropy = 0.0;
    long double entropyR = 0.0;
    long double entropyG = 0.0;
    long double entropyB = 0.0;

    for (int i = 0; i < vProb.size(); i++)
    {
        long double prob = (long double)vProb[i].count/(width*height*3);
        long double probR = (long double)vProbR[i].count/(width*height);
        long double probG = (long double)vProbG[i].count/(width*height);
        long double probB = (long double)vProbB[i].count/(width*height);

        if (prob != 0)
        {
            entropy += (prob * log2(1 / prob));
        }
        if (probR != 0)
        {
            entropyR += (probR * log2(1 / probR));
        }
        if (probG != 0)
        {
            entropyG += (probG * log2(1 / probG));
        }
        if (probB != 0)
        {
            entropyB += (probB * log2(1 / probB));
        }
    }

    cout << "Predictor nr:\t" << option << "\n" << "General:\t" << entropy << "\nRed:\t\t" << entropyR << "\nGreen:\t\t" << entropyG << "\nBlue:\t\t" << entropyB << endl;
    return 0;
}