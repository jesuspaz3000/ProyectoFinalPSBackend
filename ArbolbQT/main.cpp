#include <iostream>
#include <string>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include "btree.h"

using namespace std;

BTree* t = nullptr; // Cambiar a puntero para poder inicializar dinámicamente

QJsonObject readJsonFile(const QString &filename) {
    QFile file(filename);
    QJsonObject jsonObject;
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        jsonObject = doc.object();
        file.close();
    }
    return jsonObject;
}

void writeJsonFile(const QString &filename, const QJsonObject &jsonObject) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(jsonObject);
        file.write(doc.toJson());
        file.close();
    }
}

void initializeTree(int degree, const QString &filename) {
    t = new BTree(degree);
    QJsonObject jsonObject;
    jsonObject["degree"] = degree;
    writeJsonFile(filename, jsonObject);
}

void loadTree(const QString &filename) {
    QJsonObject jsonObject = readJsonFile(filename);
    if (!jsonObject.isEmpty()) {
        int degree = jsonObject["degree"].toInt();
        t = new BTree(degree);
    }
}

QString handleInsert(const string &word, const QString &filename) {
    QJsonObject jsonObject = readJsonFile(filename);
    QJsonArray keys = jsonObject["keys"].toArray();

    if (!keys.contains(QString::fromStdString(word))) {
        keys.append(QString::fromStdString(word));
        jsonObject["keys"] = keys;
        writeJsonFile(filename, jsonObject);
    }

    QJsonObject response;
    response["action"] = "insert";
    response["key"] = QString::fromStdString(word);
    QJsonDocument doc(response);
    QString output = doc.toJson(QJsonDocument::Compact);
    return output;
}

QString handleSearch(const string &word, const QString &filename) {
    QJsonObject jsonObject = readJsonFile(filename);
    QJsonArray keys = jsonObject["keys"].toArray();

    bool found = keys.contains(QString::fromStdString(word));
    QJsonObject response;
    response["action"] = "search";
    response["key"] = QString::fromStdString(word);
    response["found"] = found;
    QJsonDocument doc(response);
    QString output = doc.toJson(QJsonDocument::Compact);
    return output;
}

QString handleDelete(const string &word, const QString &filename) {
    QJsonObject jsonObject = readJsonFile(filename);
    QJsonArray keys = jsonObject["keys"].toArray();

    QJsonArray newKeys;
    for (const QJsonValue &key : keys) {
        if (key.toString() != QString::fromStdString(word)) {
            newKeys.append(key);
        }
    }
    jsonObject["keys"] = newKeys;
    writeJsonFile(filename, jsonObject);

    QJsonObject response;
    response["action"] = "delete";
    response["key"] = QString::fromStdString(word);
    QJsonDocument doc(response);
    QString output = doc.toJson(QJsonDocument::Compact);
    return output;
}

QString handleTraverse(const QString &filename) {
    QJsonObject jsonObject = readJsonFile(filename);
    QJsonArray keys = jsonObject["keys"].toArray();

    QJsonObject response;
    response["action"] = "traverse";
    response["keys"] = keys;
    QJsonDocument doc(response);
    QString output = doc.toJson(QJsonDocument::Compact);
    return output;
}

int main(int argc, char* argv[]) {
    const QString filename = "btree_data.json";

    if (argc > 1) {
        string command = argv[1];
        if (command == "init" && argc == 3) {
            int degree = stoi(argv[2]);
            initializeTree(degree, filename);
            cout << "Initialized BTree with degree " << degree << endl;
        } else {
            loadTree(filename);
            if (t == nullptr) {
                cout << "BTree not initialized. Use the init command to initialize it with a degree." << endl;
                return 1;
            }
            if (command == "insert" && argc == 3) {
                cout << handleInsert(argv[2], filename).toStdString() << endl;
            } else if (command == "search" && argc == 3) {
                cout << handleSearch(argv[2], filename).toStdString() << endl;
            } else if (command == "delete" && argc == 3) {
                cout << handleDelete(argv[2], filename).toStdString() << endl;
            } else if (command == "traverse") {
                cout << handleTraverse(filename).toStdString() << endl;
            } else {
                cout << "Invalid command" << endl;
            }
        }
    }

    return 0;
}
