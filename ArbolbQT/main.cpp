#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include "btree.h"

const QString CSV_FILE = QDir::currentPath() + "/btree_data.csv";
const QString JSON_FILE = QDir::currentPath() + "/btree_data.json";

QJsonObject treeToJson(BTreeNode* node) {
    QJsonObject obj;
    if (node == nullptr) {
        return obj;
    }
    QJsonArray keysArray;
    for (int i = 0; i < node->n; i++) {
        keysArray.append(node->keys[i]);
    }
    obj["keys"] = keysArray;
    obj["leaf"] = node->leaf;
    if (!node->leaf) {
        QJsonArray childrenArray;
        for (int i = 0; i <= node->n; i++) {
            childrenArray.append(treeToJson(node->children[i]));
        }
        obj["children"] = childrenArray;
    }
    return obj;
}

void saveTreeToCSV(int minDegree, const QVector<int>& insertedKeys) {
    QFile file(CSV_FILE);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo" << CSV_FILE;
        return;
    }

    QTextStream out(&file);
    out << minDegree << ",";
    for (int i = 0; i < insertedKeys.size(); ++i) {
        out << insertedKeys[i];
        if (i < insertedKeys.size() - 1) out << ";";
    }
    out << "\n";

    file.close();
}

void saveTreeToJSON(BTree* tree) {
    QJsonObject jsonTree;
    jsonTree["tree"] = treeToJson(tree->getRoot());
    QJsonDocument doc(jsonTree);
    QFile jsonFile(JSON_FILE);
    if (jsonFile.open(QIODevice::WriteOnly)) {
        jsonFile.write(doc.toJson(QJsonDocument::Indented));
        jsonFile.close();
        qDebug() << "JSON guardado en:" << JSON_FILE;
    } else {
        qDebug() << "No se pudo abrir el archivo" << JSON_FILE << "para escribir";
    }
}

QPair<int, QVector<int>> loadFromCSV() {
    QFile file(CSV_FILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo" << CSV_FILE;
        return qMakePair(0, QVector<int>());
    }

    QTextStream in(&file);
    QString line = in.readLine();
    QStringList parts = line.split(',');
    if (parts.size() != 2) {
        qDebug() << "Formato de CSV inválido";
        file.close();
        return qMakePair(0, QVector<int>());
    }

    int minDegree = parts[0].toInt();
    QStringList keyStrings = parts[1].split(';');
    QVector<int> keys;
    for (const QString& keyStr : keyStrings) {
        if (!keyStr.isEmpty()) {
            keys.append(keyStr.toInt());
        }
    }

    file.close();
    return qMakePair(minDegree, keys);
}

void printUsage() {
    qDebug() << "Uso: ./ArbolbQT <comando> [argumento]";
    qDebug() << "Comandos disponibles:";
    qDebug() << "  init <grado_minimo>   - Inicializa el árbol B con el grado mínimo especificado";
    qDebug() << "  insert <valor>        - Inserta un valor en el árbol";
    qDebug() << "  search <valor>        - Busca un valor en el árbol";
    qDebug() << "  delete <valor>        - Elimina un valor del árbol";
    qDebug() << "  traverse              - Imprime los valores del árbol en orden";
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("comando", "Comando a ejecutar");
    parser.addPositionalArgument("argumento", "Argumento del comando (opcional)");

    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        printUsage();
        return 0;
    }

    QString comando = args.at(0);
    BTree* arbol = nullptr;
    int minDegree;
    QVector<int> insertedKeys;

    if (comando != "init") {
        auto loadedData = loadFromCSV();
        minDegree = loadedData.first;
        insertedKeys = loadedData.second;
        if (minDegree < 2) {
            qDebug() << "Error: Datos del árbol no inicializados. Use 'init' primero.";
            return 1;
        }
        arbol = new BTree(minDegree);
        for (int key : insertedKeys) {
            arbol->insert(key);
        }
    }

    QJsonObject result;
    if (comando == "init") {
        if (args.size() < 2) {
            qDebug() << "Error: El comando 'init' requiere un argumento.";
            return 1;
        }
        minDegree = args.at(1).toInt();
        if (minDegree < 2) {
            qDebug() << "Error: El grado mínimo debe ser al menos 2.";
            return 1;
        }
        delete arbol;
        arbol = new BTree(minDegree);
        insertedKeys.clear();
        saveTreeToCSV(minDegree, insertedKeys);
        saveTreeToJSON(arbol);
        result["message"] = "Árbol B inicializado con grado mínimo " + QString::number(minDegree);
    } else if (comando == "insert") {
        if (args.size() < 2) {
            qDebug() << "Error: El comando 'insert' requiere un argumento.";
            return 1;
        }
        int valor = args.at(1).toInt();
        arbol->insert(valor);
        if (!insertedKeys.contains(valor)) {
            insertedKeys.append(valor);
        }
        saveTreeToCSV(minDegree, insertedKeys);
        saveTreeToJSON(arbol);
        result["message"] = "Insertado: " + QString::number(valor);
    } else if (comando == "search") {
        if (args.size() < 2) {
            qDebug() << "Error: El comando 'search' requiere un argumento.";
            return 1;
        }
        int valor = args.at(1).toInt();
        BTreeNode* resultado = arbol->search(valor);
        result["found"] = resultado != nullptr;
        result["message"] = resultado ? "Encontrado: " + QString::number(valor) : "No encontrado: " + QString::number(valor);
    } else if (comando == "delete") {
        if (args.size() < 2) {
            qDebug() << "Error: El comando 'delete' requiere un argumento.";
            return 1;
        }
        int valor = args.at(1).toInt();
        arbol->remove(valor);
        insertedKeys.removeOne(valor);
        saveTreeToCSV(minDegree, insertedKeys);
        saveTreeToJSON(arbol);
        result["message"] = "Eliminado (si existía): " + QString::number(valor);
    } else if (comando == "traverse") {
        QVector<int> traverseResult;
        arbol->inorderTraversal(traverseResult);
        QJsonArray jsonArray;
        for (int value : traverseResult) {
            jsonArray.append(value);
        }
        result["traverse"] = jsonArray;
        result["message"] = "Recorrido en orden completado";
    } else {
        qDebug() << "Comando no reconocido:" << comando;
        printUsage();
        return 1;
    }

    // Imprimir el resultado como JSON en la salida estándar
    QJsonDocument doc(result);
    printf("%s\n", doc.toJson(QJsonDocument::Compact).constData());
    fflush(stdout);

    delete arbol;
    return 0;
}
