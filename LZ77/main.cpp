#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define CODE_BUFF_SIZE 256
#define DIC_BUFF_SIZE 255

using namespace std;

FILE *in, *out;
unsigned char * buff;
long fileLength;
int bitBuff = 0, bitMask = 128;
unsigned long codeLength = 0;

void getZero()
{
    if((bitMask >>= 1) == 0)
    {
        fputc(bitBuff, out);
        bitBuff = 0;
        bitMask = 128;
        codeLength++;
    }
}

void getOne()
{
    bitBuff |= bitMask;

    if((bitMask >>= 1) == 0)
    {
        fputc(bitBuff, out);
        bitBuff = 0;
        bitMask = 128;
        codeLength++;
    }
}

void outputChar(int code)
{
    int mask = 256;
    getZero();

    while(mask >>= 1)
    {
        if(code & mask)
            getOne();
        else
            getZero();
    }
}

void outputMatched(int leftShift, int matchLength)
{
    int mask;
    getOne();

    mask =(1 << 8);
    while(mask >>= 1)
    {
        if(leftShift & mask)
            getOne();
        else
            getZero();
    }

    mask =(1 << 8);
    while(mask >>= 1)
    {
        if(matchLength & mask)
            getOne();
        else
            getZero();
    }
}

void flushBuffer()
{
    if(bitMask != 128)
    {
        fputc(bitBuff, out);
        codeLength++;
    }
}

void encode()
{
    fseek(in, 0, SEEK_END);
    fileLength = ftell(in);
    rewind(in);
    buff = reinterpret_cast<unsigned char *>(malloc(fileLength + 1));
    fread(buff, fileLength, 1, in);
    int buffLength = 0;

    for(int offset = 0; offset < fileLength;)
    {
        int maxMatch = 0;
        int leftShift = 0;

        for(int i = 1; i <= buffLength; i++)
        {
            int currMatch = 0;

            while(buff[offset - i + currMatch] == buff[offset + currMatch] && currMatch < CODE_BUFF_SIZE)
                currMatch++;

            if(currMatch > maxMatch)
            {
                maxMatch = currMatch;
                leftShift = i;
            }
        }

        if(buffLength < DIC_BUFF_SIZE)
            buffLength++;

        if(maxMatch <= 1)
        {
            outputChar(buff[offset]);
            maxMatch = 1;
        }
        else
        {
            outputMatched(leftShift, maxMatch - 1);
        }

        offset += maxMatch;
    }
    flushBuffer();
    
    cout << "Text length:\t\t" << fileLength << " bytes" << endl;
    cout << "Code length:\t\t" << codeLength << " bytes" << endl;
    cout << "Compression level:\t" << (codeLength * 100) / fileLength << "%" << endl;
}

int getBits(int n)
{
    static int buffer, mask = 0;
    int shift = 0;

    for(int i = 0; i < n; i++)
    {
        if(mask == 0)
        {
            if((buffer = fgetc(in)) == EOF)
                return EOF;
            mask = 128;
        }

        shift <<= 1;
        if(buffer & mask)
            shift++;

        mask >>= 1;
    }

    return shift;
}

void decode()
{
    int leftShift, matchLength, c, currPos = 0;
    int size = 16384;
    auto *buffer = static_cast<unsigned char *>(malloc(size));

    while((c = getBits(1)) != EOF)
    {
        if(!c)
        {
            if((c = getBits(8)) == EOF)
                break;

            fputc(c, out);

            if(size <= currPos)
            {
                size *= 2;
                buffer = static_cast<unsigned char *>(realloc(buffer, size));
            }

            buffer[currPos++] = c;
        }
        else
        {
            if((leftShift = getBits(8)) == EOF)
                break;
            if((matchLength = getBits(8)) == EOF)
                break;

            int startingOffset = currPos - leftShift;

            for(int i = 0; i <= matchLength; i++)
            {
                c = buffer[startingOffset + i];
                fputc((unsigned int) c, out);

                if(size <= currPos)
                {
                    size *= 2;
                    buffer = static_cast<unsigned char *>(realloc(buffer, size));
                }

                buffer[currPos++] = c;
            }
        }
    }
}


int main(int argc, char ** argv)
{
    in = fopen(argv[2] , "rb");
    out = fopen(argv[3], "wb");

    char * s = argv[1];
    if(*s == 'e') {
        encode();
        fclose(out);
        out = fopen(argv[3], "rb");
    }
    else if(*s == 'd')
        decode();

    fclose(in);
    fclose(out);

    return 0;
}