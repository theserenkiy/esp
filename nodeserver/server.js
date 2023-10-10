const port = 5555;

process.title = "SERVER @ "+port;

const cl = console.log;

import path from 'path';
import express from "express";

const app = express();

app.get('/',(req, res) => {
	res.removeHeader('Transfer-Encoding');
    res.removeHeader('X-Powered-By');
	res.removeHeader('Date');
	res.removeHeader('Content-Type');
	// res.type('text');
	let s = JSON.stringify(req.query)+"";
	res.writeHead(200,{"Content-Length": Buffer.byteLength(s)});
	
	res.write(s);
	return;
})

app.get("*",(req, res) => {
	res.sendStatus(404);
})

app.listen(port, ()=>cl('Listening on port '+port));

