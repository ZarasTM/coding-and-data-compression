#ifndef KDDLIST2_HUFFMANTREE_H
#define KDDLIST2_HUFFMANTREE_H
#include <string>
#include <vector>
#include "BitChar.h"

struct Node {
    int num;            // Number in a tree (you can't be 0th node)
    std::string key;    // Held symbol can be looked as a key
    int weight;         // Sum of children's weights

    Node* p;
    Node* left;
    Node* right;
};

class HuffmanTree {
public:
    std::string output = "";

    HuffmanTree();

    ~HuffmanTree();

    Node* getNodeFromCode(std::string code);

    std::string getCode(Node* node);

    void encryptSymbol(std::string binString);

    void decryptSymbol(std::string binString);

    Node* getNyt();

private:
    Node* root;
    Node* nyt;
    int total = 500;
    std::vector<Node*> nodes;


    void destroyTree(Node* node);

    Node* searchTree(std::string key);

    Node* insertNode(std::string key);

    Node* getMaxNode(int weight);

    void updateTree(Node* node);

    void swap(Node* node1, Node* node2);
};



#endif //KDDLIST2_HUFFMANTREE_H
