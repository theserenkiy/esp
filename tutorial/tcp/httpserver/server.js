const port = 5555;

process.title = "HTTP SERVER @ "+port;

const cl = console.log;

import path from 'path';
import express from "express";
import fs from 'fs';

const app = express();
app.use(function(req, res, next) {
	req.rawBody = '';
	req.setEncoding('utf8');

	req.on('data', function(chunk) { 
		//cl("chunk: ",chunk)
		req.rawBody += chunk;
	});

	req.on('end', function() {
		next();
	});
});
app.get('/echo',(req, res) => {
	console.log("New request",req.query);
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

app.get('/',(req, res) => {
	console.log("New request",req.query);
	res.removeHeader('Transfer-Encoding');
    res.removeHeader('X-Powered-By');
	res.removeHeader('Date');
	res.removeHeader('Content-Type');
	// res.type('text');
	let s = fs.readFileSync('files/'+req.query.id+'.bin');
	res.writeHead(200,{"Content-Length": Buffer.byteLength(s)});
	
	res.write(s);
	return;
})

app.post("/",(req, res) => {
	if(!req.query.id)
	{
		res.send('Wrong id');
		return;
	}
	fs.writeFileSync('files/'+req.query.id+'.bin',req.rawBody);
	res.send('Ok');
})

app.listen(port, ()=>cl('Listening on port '+port));

