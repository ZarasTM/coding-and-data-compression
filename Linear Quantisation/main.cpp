#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#define HEADER_SIZE 18
#define FOOTER_SIZE 26

using namespace std;

struct PIXEL{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

int colorBits[] = {1, 1, 1}; // {R, G, B} default is 8
char* fileBuffer; // 18 bytes header, 26 bytes footer

unsigned short width;
unsigned short height;

PIXEL** imgOld;
PIXEL** imgNew;

void getAdditionalData(){
    /*
     * MSE
     */

    double mseR = 0;
    double mseG = 0;
    double mseB = 0;

    // Sum
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            mseR = mseR + pow((imgOld[i][j].R - imgNew[i][j].R), 2);
            mseG = mseG + pow((imgOld[i][j].G - imgNew[i][j].G), 2);
            mseB = mseB + pow((imgOld[i][j].B - imgNew[i][j].B), 2);
        }
    }

    // Divide
    mseR = mseR / (width * height);
    mseG = mseG / (width * height);
    mseB = mseB / (width * height);

    // Count res for all
    double mseAll = (mseR + mseG + mseB) / 3;

    // Print results
    cout << "MSE:\t" << mseAll << endl;
    cout << "MSE R:\t" << mseR << endl;
    cout << "MSE G:\t" << mseG << endl;
    cout << "MSE B:\t" << mseB << endl;

    /*
     * SNR
     */

    double snrR = 0;
    double snrG = 0;
    double snrB = 0;

    // Sum
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            snrR = snrR + pow(imgOld[i][j].R, 2);
            snrG = snrG + pow(imgOld[i][j].G, 2);
            snrB = snrB + pow(imgOld[i][j].B, 2);
        }
    }

    // Divide
    snrR = snrR / (width * height);
    snrG = snrG / (width * height);
    snrB = snrB / (width * height);

    // Count res for all
    double snrAll = (snrR + snrG + snrB) / 3;

    // Final dividing
    snrAll = snrAll / mseAll;
    snrR = snrR / mseR;
    snrG = snrG / mseG;
    snrB = snrB / mseB;

    // Print results
    cout << "SNR:\t" << snrAll << "\t(" << (10 * log(snrAll)) << " dB)" << endl;
    cout << "SNR R:\t" << snrR << "\t(" << (10 * log(snrR)) << " dB)" << endl;
    cout << "SNR G:\t" << snrG << "\t(" << (10 * log(snrG)) << " dB)" << endl;
    cout << "SNR B:\t" << snrB << "\t(" << (10 * log(snrB)) << " dB)" << endl;
}

PIXEL shiftBits(PIXEL color){

	if(colorBits[0] != 8){
	    color.R = color.R >> (8 - colorBits[0] - 1);
	    color.R = color.R | 1;
	    color.R = color.R << (8 - colorBits[0] - 1);
	}

	if(colorBits[1] != 8){
    	color.G = color.G >> (8 - colorBits[1] - 1);
    	color.G = color.G | 1;
    	color.G = color.G << (8 - colorBits[1] - 1);
	}

	if(colorBits[0] != 8){
    	color.B = color.B >> (8 - colorBits[2] - 1);
    	color.B = color.B | 1;
    	color.B = color.B << (8 - colorBits[2] - 1);
	}
    return color;
}

void quantize(){
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            imgNew[i][j] = shiftBits(imgOld[i][j]);
        }
    }
}

void imgIn(string path){
    ifstream inFile(path, ios::binary | ios_base::in);
    inFile >> noskipws;

    if(!inFile.is_open()){
        fprintf(stderr, "ERROR: Cannot read from file");
        exit(1);
    }

    auto * temp = static_cast<unsigned short *>(malloc(sizeof(unsigned short)));

    inFile.read(fileBuffer, HEADER_SIZE);

    // Getting width 8+4'th byte
    inFile.seekg(12);
    inFile.read(reinterpret_cast<char *>(temp), sizeof(unsigned short));
    width = *temp;

    // Getting height (next two bytes)
    inFile.read(reinterpret_cast<char *>(temp), sizeof(unsigned short));
    height = *temp;
    free(temp);

    // Initialize imgOld and imgNew
    imgOld = new PIXEL*[width];
    imgNew = new PIXEL*[width];
    for(int i = 0; i < width; ++i) {
        imgOld[i] = new PIXEL[height];
        imgNew[i] = new PIXEL[height];
    }

    // Load img to oldImg
    auto *color = static_cast<char *>(malloc(sizeof(char)));
    inFile.seekg(HEADER_SIZE+1);

    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            inFile.read(color, sizeof(unsigned char));
            imgOld[i][j].R = static_cast<unsigned char>(*color);

            inFile.read(color, sizeof(unsigned char));
            imgOld[i][j].G = static_cast<unsigned char>(*color);

            inFile.read(color, sizeof(unsigned char));
            imgOld[i][j].B = static_cast<unsigned char>(*color);
        }
    }

    // Place in buffer
    inFile.read(fileBuffer + HEADER_SIZE, FOOTER_SIZE);

    inFile.close();
}

void imgOut(string path){
    ofstream outFile(path);
    outFile << noskipws;

    if(!outFile.is_open()){
        fprintf(stderr, "ERROR: Cannot write to file");
        exit(1);
    }

    // Get header from buffer
    int buffIter = 0;
    while(buffIter < HEADER_SIZE){
        outFile << fileBuffer[buffIter];
        buffIter++;
    }

    // Get color data
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            outFile << imgNew[i][j].B;
            outFile << imgNew[i][j].R;
            outFile << imgNew[i][j].G;
        }
    }

    // Get footer from buffer
    while(buffIter < (18 + 26)){
        outFile << fileBuffer[buffIter];
        buffIter++;
    }

    outFile.close();
}

int main(int argc, char** argv) {
    fileBuffer = new char[18+26];

    colorBits[0] = atoi(argv[3]);
    colorBits[1] = atoi(argv[4]);
    colorBits[2] = atoi(argv[5]);
    imgIn(argv[1]);
    quantize();
    getAdditionalData();
    imgOut(argv[2]);

    return 0;
}
