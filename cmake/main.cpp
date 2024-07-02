#include <iostream>
#include <string>
#include "btree.h"

using namespace std;

int main(int argc, char* argv[]) {
    BTree t(3);

    // Precargar algunos datos en el árbol
    t.insert("apple");
    t.insert("banana");
    t.insert("cherry");
    t.insert("date");
    t.insert("fig");
    t.insert("grape");
    t.insert("kiwi");
    t.insert("lemon");

    if (argc > 1) {
        string command = argv[1];
        if (command == "insert" && argc == 3) {
            t.insert(argv[2]);
            cout << "Inserted: " << argv[2] << endl;
        } else if (command == "search" && argc == 3) {
            if (t.search(argv[2])) {
                cout << "Found: " << argv[2] << endl;
            } else {
                cout << "Not Found: " << argv[2] << endl;
            }
        } else if (command == "delete" && argc == 3) {
            t.remove(argv[2]);
            cout << "Deleted: " << argv[2] << endl;
        } else if (command == "traverse") {
            t.traverse();
            cout << endl;
        } else {
            cout << "Invalid command" << endl;
        }
    }
    return 0;
}
