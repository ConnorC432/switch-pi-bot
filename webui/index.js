const express = require('express');
const { createServer } = require('http');
const WebSocket = require('ws');

const app = express();
const PORT = 3000;

app.use(express.static('public'));

const server = createServer(app);

const wss = new WebSocket.Server({ server, path: '/stream'});

function connectBackend() {
    const backendWs = new WebSocket('ws://backend:8080/stream');

    backendWs.on('open', () => console.log('Connection opened'));
    backendWs.on('close', () => {
        console.log('Connection closed, reconnecting...');
        setTimeout(connectBackend, 600);
    });
    backendWs.on('error', err => console.log(err));

    backendWs.on('message', (data) => {
        wss.clients.forEach((client) => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(data);
            }
        });
    });
}

connectBackend();

wss.on('connection', (ws) => {
    console.log('Connection connected');
    ws.on('close', () => console.log('Connection closed'));
})

server.listen(PORT, () => {
    console.log('WebUI running at http://webui:' + PORT);
})