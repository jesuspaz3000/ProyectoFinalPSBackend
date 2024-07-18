const express = require('express');
const cors = require('cors');
const { execFile } = require('child_process');
const bodyParser = require('body-parser');
const os = require('os');
const path = require('path');
const fs = require('fs').promises;
const app = express();
const port = 8000;

let executablePath;
if (os.platform() === 'win32') {
    executablePath = path.join(__dirname, '../ArbolbQT/build/Desktop_Qt_6_8_0_MinGW_64_bit-Debug/debug/ArbolbQT.exe');
} else {
    executablePath = path.join(__dirname, '../ArbolbQT/build/Desktop-Debug/ArbolbQT');
}

app.use(cors());
app.use(bodyParser.json());

function executeCommand(command, args) {
    return new Promise((resolve, reject) => {
        console.log(`Executing command: ${command} with args: ${args}`);
        execFile(executablePath, [command, ...args], (error, stdout, stderr) => {
            if (error) {
                console.error(`Error executing command: ${error.message}`);
                console.error(`Stderr: ${stderr}`);
                reject(error);
                return;
            }
            console.log(`Command output: ${stdout}`);
            try {
                const result = JSON.parse(stdout);
                resolve(result);
            } catch (err) {
                console.error(`Error parsing JSON: ${err.message}`);
                console.error(`Raw stdout: ${stdout}`);
                reject(new Error('Invalid JSON response'));
            }
        });
    });
}

app.post('/init', async (req, res) => {
    const degree = req.body.degree;
    console.log('Received init request with body:', req.body);
    if (typeof degree !== 'number' || degree < 2) {
        return res.status(400).json({ message: 'Grado no proporcionado o inválido', error: 'Bad Request' });
    }
    try {
        console.log(`Initializing B-Tree with degree: ${degree}`);
        const result = await executeCommand('init', [degree.toString()]);
        console.log('Command execution result:', result);
        const treeStructure = await getTreeStructure();
        console.log('Tree structure after initialization:', treeStructure);
        res.json({ tree: treeStructure, message: 'Árbol B inicializado correctamente' });
    } catch (error) {
        console.error(`Error during initialization: ${error.message}`);
        res.status(500).json({ message: 'Error al inicializar el árbol', error: error.message });
    }
});

app.post('/insert', async (req, res) => {
    const number = req.body.number;
    console.log('Received insert request with body:', req.body);
    if (typeof number !== 'number') {
        return res.status(400).json({ message: 'Número no proporcionado o inválido', error: 'Bad Request' });
    }
    try {
        console.log(`Attempting to insert number: ${number}`);
        await executeCommand('insert', [number.toString()]);
        const treeStructure = await getTreeStructure();
        console.log('Insertion successful, new tree structure:', treeStructure);
        res.json({ tree: treeStructure, message: 'Inserción exitosa' });
    } catch (error) {
        console.error(`Error during insertion: ${error.message}`);
        res.status(500).json({ message: 'Error al insertar en el árbol', error: error.message });
    }
});

app.post('/search', async (req, res) => {
    const number = req.body.number;
    console.log('Received search request with body:', req.body);
    if (typeof number !== 'number') {
        return res.status(400).json({ message: 'Número no proporcionado o inválido', error: 'Bad Request' });
    }
    try {
        console.log(`Searching for number: ${number}`);
        const result = await executeCommand('search', [number.toString()]);
        console.log('Search result:', result);
        res.json(result);
    } catch (error) {
        console.error(`Error during search: ${error.message}`);
        res.status(500).json({ message: 'Error al buscar en el árbol', error: error.message });
    }
});

app.post('/delete', async (req, res) => {
    const number = req.body.number;
    console.log('Received delete request with body:', req.body);
    if (typeof number !== 'number') {
        return res.status(400).json({ message: 'Número no proporcionado o inválido', error: 'Bad Request' });
    }
    try {
        console.log(`Attempting to delete number: ${number}`);
        await executeCommand('delete', [number.toString()]);
        const treeStructure = await getTreeStructure();
        console.log('Deletion successful, new tree structure:', treeStructure);
        res.json({ tree: treeStructure, message: 'Eliminación exitosa' });
    } catch (error) {
        console.error(`Error during deletion: ${error.message}`);
        res.status(500).json({ message: 'Error al eliminar en el árbol', error: error.message });
    }
});

app.get('/get_structure', async (req, res) => {
    try {
        const treeStructure = await getTreeStructure();
        console.log('Retrieved tree structure:', treeStructure);
        res.json(treeStructure);
    } catch (error) {
        console.error(`Error retrieving tree structure: ${error.message}`);
        res.status(500).json({ message: 'Error al obtener la estructura del árbol', error: error.message });
    }
});

app.get('/traverse', async (req, res) => {
    try {
        console.log('Traversing the tree');
        const result = await executeCommand('traverse', []);
        console.log('Traverse result:', result);
        res.json(result);
    } catch (error) {
        console.error(`Error during tree traversal: ${error.message}`);
        res.status(500).json({ message: 'Error al recorrer el árbol', error: error.message });
    }
});

async function getTreeStructure() {
    const jsonFilePath = path.join(__dirname, 'btree_data.json');
    try {
        const data = await fs.readFile(jsonFilePath, 'utf8');
        return JSON.parse(data);
    } catch (error) {
        console.error(`Error reading or parsing JSON file: ${error.message}`);
        throw error;
    }
}

app.listen(port, 'localhost', () => {
    console.log(`Server running at http://localhost:${port}/`);
});