#ifndef BTREE_H
#define BTREE_H

#include <string>

using namespace std;

// Definición de la estructura del nodo del árbol B
class BTreeNode {
public:
    string *keys;      // Array de claves
    int t;          // Grado mínimo
    BTreeNode **C;  // Array de hijos
    int n;          // Número actual de claves
    bool leaf;      // Verdadero si el nodo es hoja

    BTreeNode(int _t, bool _leaf);  // Constructor

    // Función para recorrer todos los nodos en un subárbol enraizado con este nodo
    void traverse();

    // Función para buscar una clave en el subárbol enraizado con este nodo
    BTreeNode *search(const string &k);

    // Función que retorna el índice de la primera clave mayor o igual a k
    int findKey(const string &k);

    // Función para insertar una nueva clave en el subárbol enraizado con este nodo
    void insertNonFull(const string &k);

    // Función para dividir el nodo hijo y que este nodo esté lleno al pasarle un índice
    void splitChild(int i, BTreeNode *y);

    // Función para eliminar la clave k en el subárbol enraizado con este nodo
    void remove(const string &k);

    // Función para remover la clave presente en la posición idx en este nodo que es una hoja
    void removeFromLeaf(int idx);

    // Función para remover la clave presente en la posición idx en este nodo que no es una hoja
    void removeFromNonLeaf(int idx);

    // Función para obtener el predecesor de las claves[idx]
    string getPred(int idx);

    // Función para obtener el sucesor de las claves[idx]
    string getSucc(int idx);

    // Función para llenar el nodo hijo C[idx] que tiene menos de t-1 claves
    void fill(int idx);

    // Función para tomar una clave del nodo hermano anterior de C[idx]
    void borrowFromPrev(int idx);

    // Función para tomar una clave del nodo hermano siguiente de C[idx]
    void borrowFromNext(int idx);

    // Función para unir C[idx] con C[idx+1]
    void merge(int idx);

    // Declarar la clase BTree como amiga para permitir el acceso a sus miembros privados
    friend class BTree;
};

// Definición de la estructura del árbol B
class BTree {
public:
    BTreeNode *root;  // Puntero a la raíz
    int t;            // Grado mínimo

    BTree(int _t) {
        root = nullptr;
        t = _t;
    }

    // Función para recorrer el árbol
    void traverse();

    // Función para buscar una clave en el árbol
    BTreeNode* search(const string &k);

    // Función para insertar una nueva clave en el árbol
    void insert(const string &k);

    // Función para eliminar una clave del árbol
    void remove(const string &k);
};

#endif // BTREE_H
