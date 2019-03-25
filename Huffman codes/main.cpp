#include <utility>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include "HuffmanTree.h"
#include "BitChar.h"

using namespace std;

static vector<char> readAllBytesFrom(const string &filename)
{
    ifstream ifs(filename, ios::binary|ios::ate);
    ifstream::pos_type pos = ifs.tellg();

    vector<char> result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);

    return result;
}

void encrypt(string pathIn, string pathOut)
{
    HuffmanTree* tree = new HuffmanTree();

    vector<char> rawFile = readAllBytesFrom(pathIn);

    BitChar* bChar = new BitChar();
    string resSym;

    for(auto &sym : rawFile)
    {
        resSym = bChar->getBits(sym);
        tree->encryptSymbol(resSym);
    }

    bChar->setBits(tree->output);
    ofstream outf;
    outf.open(pathOut);
    bChar->insertBits(outf);
    outf.close();
}

void decrypt(string pathIn, string pathOut)
{
    HuffmanTree* tree = new HuffmanTree();
    vector<char> rawFile = readAllBytesFrom(pathIn);
    BitChar* bChar = new BitChar();
    string rawFileString = "";
    string output = "";

    for(auto &c : rawFile)
    {
        rawFileString.append(bChar->getBits(c));
    }

    string code = "";
    Node* temp;

    // Resolve first unencrypted 8 bit symbol
    tree->decryptSymbol(rawFileString.substr(0, 8));
    output.append(rawFileString.substr(0, 8));
    rawFileString = rawFileString.substr(8);
    unsigned int i;

    while(rawFileString.length() != 0) // Repeat until we have nothing to decode
    {
        for(i=1; i<=rawFileString.length(); i++) // Find next coded node
        {
            temp = tree->getNodeFromCode(rawFileString.substr(0, i));

            if(temp != nullptr && temp->left == nullptr && temp->right == nullptr) // If node is not connector or root
            {
                rawFileString = rawFileString.substr(i); // Delete coding for given node

                if(temp == tree->getNyt()) // If node is NYT
                {
                    tree->decryptSymbol(rawFileString.substr(0, 8)); // Get 8 bit symbol from code
                    output.append(rawFileString.substr(0, 8));
                    rawFileString = rawFileString.substr(8);
                    break;
                }
                else
                {
                    tree->decryptSymbol(temp->key);
                    output.append(temp->key);
                    break;
                }
            }

            if(i == rawFileString.length() && temp->left != nullptr)
            {
                break;
            }
        }

        if(i == rawFileString.length() && temp->left != nullptr)
        {
            break;
        }

    }

   // Delete last zeroes
    /*unsigned long i = rawFileString.length() - 1;
    while(rawFileString[i] == '0') i--;
    rawFileString = rawFileString.substr(0, i+1);

    unsigned long i;
    // Resolve first unencrypted 8 bit symbol
    tree->decryptSymbol(rawFileString.substr(0, 8));
    output.append(rawFileString.substr(0, 8));
    rawFileString = rawFileString.substr(8);

    Node* temp;
    while(rawFileString.length() != 0)
    {
        i = 0;
        while(tree->getNodeFromCode(rawFileString.substr(0, i)) != nullptr && i != rawFileString.length()) i++;

        if(i == rawFileString.length()) i++;

        temp = tree->getNodeFromCode(rawFileString.substr(0, i - 1));

        if(temp == tree->getNyt())
        {
            tree->decryptSymbol(rawFileString.substr(i-1, 8));
            output.append(rawFileString.substr(i-1, 8));
            rawFileString = rawFileString.substr(i+7);
        }
        else
        {
            rawFileString = rawFileString.substr(i-1);
            output.append(temp->key);
        }
    }*/

    bChar->setBits(output);
    ofstream outf;
    outf.open(pathOut);
    bChar->insertBits(outf);
    outf.close();

    //cout << output << endl;
}

int main()
{
    encrypt("/root/CLionProjects/KDDlist2/pan-tadeusz.txt", "/root/CLionProjects/KDDlist2/enc.bin");
    decrypt("/root/CLionProjects/KDDlist2/enc.bin", "/root/CLionProjects/KDDlist2/dec");
    return 0;
}