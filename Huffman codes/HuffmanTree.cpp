#include <fstream>
#include <iostream>
#include "HuffmanTree.h"

using namespace std;

// Constructor and Destructor
HuffmanTree::HuffmanTree()
{
    this->root = nullptr;
    this->nyt = new Node();

    nyt->key = "NYT";
    nyt->num = 1;
    nyt->weight = 0;
    nyt->p = nullptr;
    nyt->left = nullptr;
    nyt->right = nullptr;
}

HuffmanTree::~HuffmanTree()
{
    destroyTree(root);
}

void HuffmanTree::encryptSymbol(string binString)
{
    Node* temp = searchTree(binString);

    if(temp == nullptr)
    {
        output.append(getCode(nyt)).append(binString);
        temp = insertNode(binString);
        nodes.push_back(temp);
    }
    else
    {
        output += getCode(temp);
        temp->weight++;
    }
    updateTree(temp);
}

void HuffmanTree::decryptSymbol(string binString)
{
    Node* temp = searchTree(binString);

    if(temp == nullptr)
    {
        temp = insertNode(binString);
        nodes.push_back(temp);
    }
    else
    {
        temp->weight++;
    }
    updateTree(temp);
}

Node* HuffmanTree::insertNode(string key)
{
    Node* temp = new Node;

    if(root == nullptr)
    {
        root = new Node();
        root->key = "ROOT";
        root->left = nyt;
        root->right = temp;
        root->num = total;
        root->weight = 1;

        temp->num = --total;
        temp->p = root;
        nyt->num = --total;
        nyt->p = root;

    }
    else
    {
        Node *connecter = new Node;
        connecter->key = "CONNECTER";
        connecter->left = nyt;
        connecter->right = temp;
        connecter->p = nyt->p;
        connecter->num = total;
        connecter->weight = 2;

        temp->p = connecter;
        temp->num = --total;

        nyt->p->left = connecter;
        nyt->p = connecter;
        nyt->num = --total;

        nodes.push_back(connecter);
    }

    temp->key = key;
    temp->left = nullptr;
    temp->right = nullptr;

    temp->weight = 1;

    return temp;
}

void HuffmanTree::updateTree(Node* node)
{
    Node* temp = nullptr;

    while(node != root)
    {
        temp = getMaxNode(node->weight);

        if(temp != nyt && temp != node && temp != node->p && temp != nullptr)
        {
            // Exchange numbers
            int tempNum = temp->num;
            temp->num = node->num;
            node->num = tempNum;

            swap(temp, node);
        }

        if(node->left != nullptr && node->right != nullptr)
        {
            node->weight = node->left->weight + node->right->weight;
        }
        else
        {
            node->weight++;
        }

        node = node->p;
    }

    root->weight = root->left->weight + root->right->weight;
}

Node* HuffmanTree::getNodeFromCode(string code)
{
    Node* temp = root;
    for(auto &c : code)
    {
        if(temp == nullptr)
        {
            return temp;
        }
        if(c == '1')
        {
            temp = temp->right;
        }
        else if (c == '0')
        {
            temp = temp->left;
        }
    }

    return temp;
}

string HuffmanTree::getCode(Node* node)
{
    string code;

    while(node->p != nullptr)
    {
        if(node == node->p->left) code = "0"+code;
        else if(node == node->p->right) code = "1"+code;
        node = node->p;
    }

    return code;
}

void HuffmanTree::destroyTree(Node* node)
{
    if(node != nullptr)
    {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

Node* HuffmanTree::searchTree(string key)
{
    for (auto &node : nodes)
    {
        if(node->key == key)
        {
            return node;
        }
    }
    return nullptr;
}

Node* HuffmanTree::getMaxNode(int weight)
{
    Node* maxNode = nyt;

    for(auto &temp : nodes)
    {
        if (temp->weight == weight)
        {
            if (maxNode->num < temp->num)
            {
                maxNode = temp;
            }
        }
    }

    return maxNode;
}

void HuffmanTree::swap(Node* node1, Node* node2)
{
    if(node1 == node1->p->left)
    {
        node1->p->left = node2;
    }
    else
    {
        node1->p->right = node2;
    }

    if(node2 == node2->p->left)
    {
        node2->p->left = node1;
    }
    else
    {
        node2->p->right = node1;
    }

    Node* temp = node1->p;

    node1->p = node2->p;
    node2->p = temp;
}

Node* HuffmanTree::getNyt(){ return nyt;}