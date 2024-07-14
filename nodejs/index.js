const express = require('express');
const cors = require('cors');
const { execFile } = require('child_process');
const bodyParser = require('body-parser');
const os = require('os');
const path = require('path');
const app = express();
const port = 8000;
const host = '0.0.0.0';  // Escuchar en todas las interfaces de red

// Determinar el sistema operativo y configurar la ruta del ejecutable
let executablePath;
if (os.platform() === 'win32') {
    executablePath = path.join(__dirname, '../cmake/build/Debug/arbolb.exe');
} else {
    executablePath = path.join(__dirname, '../ArbolbQT/build/Desktop-Debug/ArbolbQT');
}

app.use(cors()); // Habilitar CORS
app.use(bodyParser.json());

app.post('/init', (req, res) => {
    const degree = req.body.degree;
    console.log(`Initializing BTree with degree: ${degree}`);
    execFile(executablePath, ['init', degree.toString()], (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${stderr}`);
            res.status(500).json({ message: '', error: stderr });
            return;
        }
        console.log(`Result: ${stdout}`);
        res.json({ message: `Initialized BTree with degree ${degree}` });
    });
});

app.post('/insert', (req, res) => {
    const word = req.body.word;
    console.log(`Inserting word: ${word}`);
    execFile(executablePath, ['insert', word], (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${stderr}`);
            res.status(500).json({ message: '', error: stderr });
            return;
        }
        console.log(`Result: ${stdout}`);
        res.json(JSON.parse(stdout));
    });
});

app.post('/search', (req, res) => {
    const word = req.body.word;
    console.log(`Searching word: ${word}`);
    execFile(executablePath, ['search', word], (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${stderr}`);
            res.status(500).json({ message: '', error: stderr });
            return;
        }
        const result = JSON.parse(stdout);
        console.log(`Result: ${stdout}`);
        res.json(result);
    });
});

app.post('/delete', (req, res) => {
    const word = req.body.word;
    console.log(`Deleting word: ${word}`);
    execFile(executablePath, ['delete', word], (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${stderr}`);
            res.status(500).json({ message: '', error: stderr });
            return;
        }
        console.log(`Result: ${stdout}`);
        res.json(JSON.parse(stdout));
    });
});

app.get('/traverse', (req, res) => {
    console.log('Traversing tree');
    execFile(executablePath, ['traverse'], (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${stderr}`);
            res.status(500).json({ keys: [], error: stderr });
            return;
        }
        if (stdout.trim() === "") {
            res.json({ keys: [] });
        } else {
            console.log(`Result: ${stdout}`);
            res.json(JSON.parse(stdout));
        }
    });
});

app.listen(port, host, () => {
    console.log(`Server running at http://${host}:${port}/`);
});
