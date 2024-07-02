#include "btree.h"
#include <iostream>

using namespace std;

// Constructor para el nodo del árbol B
BTreeNode::BTreeNode(int t1, bool leaf1) {
    t = t1;
    leaf = leaf1;

    keys = new string[2 * t - 1];
    C = new BTreeNode *[2 * t];

    n = 0;
}

// Función para recorrer el árbol
void BTreeNode::traverse() {
    int i;
    for (i = 0; i < n; i++) {
        if (!leaf) {
            C[i]->traverse();
        }
        cout << " " << keys[i];
    }

    if (!leaf) {
        C[i]->traverse();
    }
}

// Función para buscar una clave en el subárbol
BTreeNode* BTreeNode::search(const string &k) {
    int i = 0;
    while (i < n && k > keys[i]) {
        i++;
    }

    if (keys[i] == k) {
        return this;
    }

    if (leaf) {
        return nullptr;
    }

    return C[i]->search(k);
}

// Función para encontrar la primera clave mayor o igual a k
int BTreeNode::findKey(const string &k) {
    int idx = 0;
    while (idx < n && keys[idx] < k) {
        ++idx;
    }
    return idx;
}

// Función para insertar una nueva clave en el subárbol
void BTree::insert(const string &k) {
    if (root == nullptr) {
        root = new BTreeNode(t, true);
        root->keys[0] = k;
        root->n = 1;
    } else {
        if (root->n == 2 * t - 1) {
            BTreeNode *s = new BTreeNode(t, false);
            s->C[0] = root;
            s->splitChild(0, root);

            int i = 0;
            if (s->keys[0] < k) {
                i++;
            }
            s->C[i]->insertNonFull(k);

            root = s;
        } else {
            root->insertNonFull(k);
        }
    }
}

// Función para insertar una clave en un nodo no lleno
void BTreeNode::insertNonFull(const string &k) {
    int i = n - 1;

    if (leaf) {
        while (i >= 0 && keys[i] > k) {
            keys[i + 1] = keys[i];
            i--;
        }

        keys[i + 1] = k;
        n = n + 1;
    } else {
        while (i >= 0 && keys[i] > k) {
            i--;
        }

        if (C[i + 1]->n == 2 * t - 1) {
            splitChild(i + 1, C[i + 1]);

            if (keys[i + 1] < k) {
                i++;
            }
        }
        C[i + 1]->insertNonFull(k);
    }
}

// Función para dividir un hijo lleno
void BTreeNode::splitChild(int i, BTreeNode *y) {
    BTreeNode *z = new BTreeNode(y->t, y->leaf);
    z->n = t - 1;

    for (int j = 0; j < t - 1; j++) {
        z->keys[j] = y->keys[j + t];
    }

    if (!y->leaf) {
        for (int j = 0; j < t; j++) {
            z->C[j] = y->C[j + t];
        }
    }

    y->n = t - 1;

    for (int j = n; j >= i + 1; j--) {
        C[j + 1] = C[j];
    }

    C[i + 1] = z;

    for (int j = n - 1; j >= i; j--) {
        keys[j + 1] = keys[j];
    }

    keys[i] = y->keys[t - 1];

    n = n + 1;
}

// Función para eliminar una clave del árbol
void BTree::remove(const string &k) {
    if (!root) {
        cout << "El arbol esta vacio\n";
        return;
    }

    root->remove(k);

    if (root->n == 0) {
        BTreeNode *tmp = root;
        if (root->leaf) {
            root = nullptr;
        } else {
            root = root->C[0];
        }
        delete tmp;
    }
}

// Función para eliminar una clave en el subárbol enraizado con este nodo
void BTreeNode::remove(const string &k) {
    int idx = findKey(k);

    if (idx < n && keys[idx] == k) {
        if (leaf) {
            removeFromLeaf(idx);
        } else {
            removeFromNonLeaf(idx);
        }
    } else {
        if (leaf) {
            cout << "La clave " << k << " no esta en el arbol\n";
            return;
        }

        bool flag = (idx == n);

        if (C[idx]->n < t) {
            fill(idx);
        }

        if (flag && idx > n) {
            C[idx - 1]->remove(k);
        } else {
            C[idx]->remove(k);
        }
    }
}

// Función para remover una clave de un nodo hoja
void BTreeNode::removeFromLeaf(int idx) {
    for (int i = idx + 1; i < n; ++i) {
        keys[i - 1] = keys[i];
    }

    n--;
}

// Función para remover una clave de un nodo no hoja
void BTreeNode::removeFromNonLeaf(int idx) {
    string k = keys[idx];

    if (C[idx]->n >= t) {
        string pred = getPred(idx);
        keys[idx] = pred;
        C[idx]->remove(pred);
    } else if (C[idx + 1]->n >= t) {
        string succ = getSucc(idx);
        keys[idx] = succ;
        C[idx + 1]->remove(succ);
    } else {
        merge(idx);
        C[idx]->remove(k);
    }
}

// Función para obtener el predecesor de una clave
string BTreeNode::getPred(int idx) {
    BTreeNode *cur = C[idx];
    while (!cur->leaf) {
        cur = cur->C[cur->n];
    }
    return cur->keys[cur->n - 1];
}

// Función para obtener el sucesor de una clave
string BTreeNode::getSucc(int idx) {
    BTreeNode *cur = C[idx + 1];
    while (!cur->leaf) {
        cur = cur->C[0];
    }
    return cur->keys[0];
}

// Función para llenar un nodo hijo
void BTreeNode::fill(int idx) {
    if (idx != 0 && C[idx - 1]->n >= t) {
        borrowFromPrev(idx);
    } else if (idx != n && C[idx + 1]->n >= t) {
        borrowFromNext(idx);
    } else {
        if (idx != n) {
            merge(idx);
        } else {
            merge(idx - 1);
        }
    }
}

// Función para tomar una clave del hermano anterior
void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx - 1];

    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
    }

    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) {
            child->C[i + 1] = child->C[i];
        }
    }

    child->keys[0] = keys[idx - 1];

    if (!leaf) {
        child->C[0] = sibling->C[sibling->n];
    }

    keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;
}

// Función para tomar una clave del hermano siguiente
void BTreeNode::borrowFromNext(int idx) {
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx + 1];

    child->keys[child->n] = keys[idx];

    if (!(child->leaf)) {
        child->C[child->n + 1] = sibling->C[0];
    }

    keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }

    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) {
            sibling->C[i - 1] = sibling->C[i];
        }
    }

    child->n += 1;
    sibling->n -= 1;
}

// Función para unir dos nodos hijos
void BTreeNode::merge(int idx) {
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx + 1];

    child->keys[t - 1] = keys[idx];

    for (int i = 0; i < sibling->n; ++i) {
        child->keys[i + t] = sibling->keys[i];
    }

    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i) {
            child->C[i + t] = sibling->C[i];
        }
    }

    for (int i = idx + 1; i < n; ++i) {
        keys[i - 1] = keys[i];
    }

    for (int i = idx + 2; i <= n; ++i) {
        C[i - 1] = C[i];
    }

    child->n += sibling->n + 1;
    n--;

    delete sibling;
}

void BTree::traverse() {
    if (root != nullptr) root->traverse();
}

BTreeNode* BTree::search(const string &k) {
    return (root == nullptr) ? nullptr : root->search(k);
}
