#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <vector>
#include <QVector>

using namespace std;

class BTreeNode {
public:
    BTreeNode(bool leaf, int t);
    void insertNonFull(int key);
    void splitChild(int i, BTreeNode* y);
    void traverse();
    BTreeNode* search(int key);
    int findKey(int key);
    void remove(int key);
    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);
    int getPred(int idx);
    int getSucc(int idx);
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);
    void inorderTraversal(QVector<int>& result) const;

    vector<int> keys;
    vector<BTreeNode*> children;
    int t;
    int n;
    bool leaf;

    friend class BTree;
};

class BTree {
public:
    BTree(int t);
    void traverse();
    BTreeNode* search(int key);
    void insert(int key);
    void remove(int key);
    void imprimirArbol() const;
    BTreeNode* getRoot() const { return root; }
    int getMinDegree() const { return t; }
    void inorderTraversal(QVector<int>& result) const;

private:
    BTreeNode* root;
    int t;
    void imprimirNivel(BTreeNode* nodo, int nivel, int indent) const;
    int altura() const;
    int alturaRecursiva(BTreeNode* nodo) const;
};

#endif // BTREE_H
