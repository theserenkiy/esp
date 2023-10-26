const port = 3021;

process.title = "HTTP MONITOR SERVER @ "+port;

const cl = console.log;
const root = path.resolve('.');

import path from 'path';
import fs from 'fs';
import express from "express";
import bodyParser from 'body-parser';

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
                let mm = fs.readFileSync(root+'/chat.txt', 'utf8')
                    .trim().split('\n').map(s => JSON.parse(s)).filter(d => d.to=='all');

                if(b.fromtime)
                    mm = mm.filter(d => d.time > b.fromtime);
                if(b.limit)
                    mm = mm.slice(-b.limit);
                out = mm;
                break;
        }
    }catch(e){
        out.error = e+'';
    }

    res.json(out)
})

app.listen(port, ()=>cl('Listening on port '+port));

