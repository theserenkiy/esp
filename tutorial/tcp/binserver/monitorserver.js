const port = 3021;

process.title = "HTTP MONITOR SERVER @ "+port;

const cl = console.log;
const root = path.resolve('.');

import path from 'path';
import fs from 'fs';
import express from "express";
import bodyParser from 'body-parser';
import { getMessages } from './lib.js';

const app = express();
app.use(bodyParser.json())
app.use(express.static(root+'/static'))

app.get('/',(req, res) => {
	res.sendFile(root+'/monitor.html');
	return;
})

app.post('/api/:cmd',(req, res) => {
    let b = req.body;
    let out = {}
    try{
        switch(req.params.cmd) {
            case 'getmessages':
                
                out = getMessages('all',b.fromtime,b.limit);
                break;
        }
    }catch(e){
        out.error = e+'';
    }

    res.json(out)
})

app.listen(port, ()=>cl('Listening on port '+port));

