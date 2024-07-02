const express = require('express');
const cors = require('cors');
const { execFile } = require('child_process');
const bodyParser = require('body-parser');
const os = require('os');
const path = require('path');
const app = express();
const port = 8000;

// Determinar el sistema operativo y configurar la ruta del ejecutable
let executablePath;
if (os.platform() === 'win32') {
    executablePath = path.join(__dirname, '../cmake/build/Debug/arbolb.exe');
} else {
    executablePath = path.join(__dirname, '../cmake/build/arbolb');
}

app.use(cors()); // Habilitar CORS
app.use(bodyParser.json());

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
        res.json({ message: `Inserted: ${word}`, error: stderr.trim() });
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
        const found = stdout.includes("Found");
        console.log(`Result: ${stdout}`);
        res.json({ message: found ? `Found: ${word}` : `Not Found: ${word}`, error: stderr.trim() });
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
        res.json({ message: stdout.trim(), error: stderr.trim() });
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
        console.log(`Result: ${stdout}`);
        const keys = stdout.trim().split(/\s+/);
        res.json({ keys, error: stderr.trim() });
    });
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}/`);
});
