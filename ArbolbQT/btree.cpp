#include "btree.h"
#include <QVector>

BTreeNode::BTreeNode(bool leaf, int t) {
    this->leaf = leaf;
    this->t = t;
    this->n = 0;
    this->keys.resize(2*t - 1);
    this->children.resize(2*t);
}

int BTreeNode::findKey(int key) {
    int idx = 0;
    while (idx < n && keys[idx] < key) {
        idx++;
    }
    return idx;
}

void BTreeNode::remove(int key) {
    int idx = findKey(key);

    if (idx < n && keys[idx] == key) {
        if (leaf) {
            removeFromLeaf(idx);
        } else {
            removeFromNonLeaf(idx);
        }
    } else {
        if (leaf) {
            cout << "La clave " << key << " no existe en el árbol.\n";
            return;
        }

        bool flag = (idx == n);

        if (children[idx]->n < t) {
            fill(idx);
        }

        if (flag && idx > n) {
            children[idx-1]->remove(key);
        } else {
            children[idx]->remove(key);
        }
    }
}

void BTreeNode::removeFromLeaf(int idx) {
    for (int i = idx + 1; i < n; ++i) {
        keys[i - 1] = keys[i];
    }
    n--;
}

void BTreeNode::removeFromNonLeaf(int idx) {
    int k = keys[idx];

    if (children[idx]->n >= t) {
        int pred = getPred(idx);
        keys[idx] = pred;
        children[idx]->remove(pred);
    } else if (children[idx + 1]->n >= t) {
        int succ = getSucc(idx);
        keys[idx] = succ;
        children[idx + 1]->remove(succ);
    } else {
        merge(idx);
        children[idx]->remove(k);
    }
}

int BTreeNode::getPred(int idx) {
    BTreeNode* cur = children[idx];
    while (!cur->leaf) {
        cur = cur->children[cur->n];
    }
    return cur->keys[cur->n - 1];
}

int BTreeNode::getSucc(int idx) {
    BTreeNode* cur = children[idx + 1];
    while (!cur->leaf) {
        cur = cur->children[0];
    }
    return cur->keys[0];
}

void BTreeNode::fill(int idx) {
    if (idx != 0 && children[idx - 1]->n >= t) {
        borrowFromPrev(idx);
    } else if (idx != n && children[idx + 1]->n >= t) {
        borrowFromNext(idx);
    } else {
        if (idx != n) {
            merge(idx);
        } else {
            merge(idx - 1);
        }
    }
}

void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx - 1];

    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
    }

    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
    }

    child->keys[0] = keys[idx - 1];

    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->n];
    }

    keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;
}

void BTreeNode::borrowFromNext(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys[(child->n)] = keys[idx];

    if (!(child->leaf)) {
        child->children[(child->n) + 1] = sibling->children[0];
    }

    keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }

    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    child->n += 1;
    sibling->n -= 1;
}

void BTreeNode::merge(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys[t - 1] = keys[idx];

    for (int i = 0; i < sibling->n; ++i) {
        child->keys[i + t] = sibling->keys[i];
    }

    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i) {
            child->children[i + t] = sibling->children[i];
        }
    }

    for (int i = idx + 1; i < n; ++i) {
        keys[i - 1] = keys[i];
    }

    for (int i = idx + 2; i <= n; ++i) {
        children[i - 1] = children[i];
    }

    child->n += sibling->n + 1;
    n--;

    delete sibling;
}

void BTreeNode::insertNonFull(int key) {
    int i = n - 1;

    if (leaf) {
        while (i >= 0 && keys[i] > key) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = key;
        n++;
    } else {
        while (i >= 0 && keys[i] > key) {
            i--;
        }
        if (children[i + 1]->n == 2 * t - 1) {
            splitChild(i + 1, children[i + 1]);
            if (keys[i + 1] < key) {
                i++;
            }
        }
        children[i + 1]->insertNonFull(key);
    }
}

void BTreeNode::splitChild(int i, BTreeNode* y) {
    BTreeNode* z = new BTreeNode(y->leaf, y->t);
    z->n = t - 1;

    for (int j = 0; j < t - 1; j++) {
        z->keys[j] = y->keys[j + t];
    }

    if (!y->leaf) {
        for (int j = 0; j < t; j++) {
            z->children[j] = y->children[j + t];
        }
    }

    y->n = t - 1;

    for (int j = n; j >= i + 1; j--) {
        children[j + 1] = children[j];
    }

    children[i + 1] = z;

    for (int j = n - 1; j >= i; j--) {
        keys[j + 1] = keys[j];
    }

    keys[i] = y->keys[t - 1];

    n++;
}

void BTreeNode::traverse() {
    int i;
    for (i = 0; i < n; i++) {
        if (!leaf) {
            children[i]->traverse();
        }
        cout << " " << keys[i];
    }
    if (!leaf) {
        children[i]->traverse();
    }
}

BTreeNode* BTreeNode::search(int key) {
    int i = 0;
    while (i < n && key > keys[i]) {
        i++;
    }

    if (keys[i] == key) {
        return this;
    }

    if (leaf) {
        return nullptr;
    }

    return children[i]->search(key);
}

void BTreeNode::inorderTraversal(QVector<int>& result) const {
    int i;
    for (i = 0; i < n; i++) {
        if (!leaf) children[i]->inorderTraversal(result);
        result.push_back(keys[i]);
    }
    if (!leaf) children[i]->inorderTraversal(result);
}

BTree::BTree(int t) {
    root = nullptr;
    this->t = t;
}

void BTree::traverse() {
    if (root != nullptr) {
        root->traverse();
    }
}

BTreeNode* BTree::search(int key) {
    return (root == nullptr) ? nullptr : root->search(key);
}

void BTree::insert(int key) {
    if (root == nullptr) {
        root = new BTreeNode(true, t);
        root->keys[0] = key;
        root->n = 1;
    } else {
        if (root->n == 2 * t - 1) {
            BTreeNode* s = new BTreeNode(false, t);
            s->children[0] = root;
            s->splitChild(0, root);

            int i = 0;
            if (s->keys[0] < key) {
                i++;
            }
            s->children[i]->insertNonFull(key);
            root = s;
        } else {
            root->insertNonFull(key);
        }
    }
}

void BTree::remove(int key) {
    if (!root) {
        cout << "El árbol está vacío\n";
        return;
    }

    root->remove(key);

    if (root->n == 0) {
        BTreeNode* tmp = root;
        if (root->leaf) {
            root = nullptr;
        } else {
            root = root->children[0];
        }
        delete tmp;
    }
}

void BTree::imprimirArbol() const {
    if (root == nullptr) {
        cout << "Árbol vacío" << endl;
        return;
    }

    cout << "Árbol B de orden " << t << ":" << endl;
    int h = altura();
    for (int i = 0; i < h; i++) {
        imprimirNivel(root, i, 0);
        cout << endl;
    }
}

void BTree::imprimirNivel(BTreeNode* nodo, int nivel, int indent) const {
    if (nodo == nullptr) return;

    if (nivel == 0) {
        cout << string(indent, ' ') << "|";
        for (int i = 0; i < nodo->n; i++) {
            cout << nodo->keys[i] << "|";
        }
    } else {
        for (int i = 0; i <= nodo->n; i++) {
            imprimirNivel(nodo->children[i], nivel - 1, indent + 4);
        }
    }
}

int BTree::altura() const {
    return alturaRecursiva(root);
}

int BTree::alturaRecursiva(BTreeNode* nodo) const {
    if (nodo == nullptr) return 0;
    if (nodo->leaf) return 1;
    return 1 + alturaRecursiva(nodo->children[0]);
}

void BTree::inorderTraversal(QVector<int>& result) const {
    if (root) root->inorderTraversal(result);
}
