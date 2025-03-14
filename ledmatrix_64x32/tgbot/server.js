const port = 8124;
process.title = "BINARY SERVER @ "+port;

const cl = console.log;

import net from 'net';
import fs from 'fs';
import fetch from 'node-fetch';
import { HttpsProxyAgent } from 'https-proxy-agent';

const bot_token = "bot8163000125:AAFkZpGtwKhd_Y4VwyhFHssAnhLP3WA9pR4";
const proxy = '192.168.88.1:1818';

let media = [];
let max_id = 0;

function readMediaDB()
{
	media = fs.existsSync('media.db') 
		? fs.readFileSync('media.db','ascii').split('\n').map(s => JSON.parse(s))
		: [];
	
	for(let d of media)
		if(d.id > max_id)
			max_id = d.id;
}

function appendMediaDB(data)
{
	if(data.id > max_id)
		max_id = data.id;
	media.push(data)
	fs.appendFileSync('media.db',JSON.stringify(data))
}

async function init()
{
	readMediaDB()
}

async function botCall(method,params={})
{
	let param_str = Object.entries(params).map(v => v[0]+'='+v[1]).join('&');
	if(param_str)param_str = '?'+param_str;
	let res = await fetch('https://api.telegram.org/'+bot_token+'/'+method+param_str, proxy ? {agent: new HttpsProxyAgent('http://'+proxy)} : {});
	let d = await res.json();
	if(!d.ok)
		throw "TG API ok=false @"+method;
	return d;
}

async function addPhoto(p)
{
	let d = await botCall('getFile',{file_id:p.file_id});
	cl(d);
}

async function getBotUpdates()
{
	let d = await botCall('getUpdates',{offset:-10});
	for(let res of d.result)
	{
		if(!res.photo && !res.video)
			continue;
		let id = res.update_id;

		if(media.find(el => el.id==id))
			continue;
		
		if(d.photo)
		{
			for(let item of res.photo)
				addPhoto(item);
		}
	}
	
	
		
	
	//cl(JSON.stringify(d,null,' '))
}

function run_server()
{
	const server = net.createServer((socket) => {
		console.log("Client connected");

		let payload_len = 0;
		let totaldata = null;
		let total_recved = 0;
		let username = null;
		let cmd = null;

		socket.on("data", (data) => {
			let out = "";
			let status = 0;
			console.log(`Received data`, data);

			try{
				cmd = data.readUInt32LE(16);

				switch(cmd)
				{
					case 1:
						out = "Server is working!";
						break;
					
					case 2:
						let last_update = getBotUpdate();
						break;
					
					case 3:
						
						break;
				}
			}
			catch(e)
			{
				cl(e)
				status = 1;
				out = (e+'').substring(0,16);
			}
			let buf = Buffer.alloc(8 + Buffer.byteLength(out));
			buf.writeInt32LE(status);
			buf.writeUInt32LE(Buffer.byteLength(out),4);

			cl('resp header', buf.subarray(0,8))
			cl('Out: ',out)

			if(out.length)
			{
				let respbuf = Buffer.from(out);
				respbuf.copy(buf,8);
			}
			socket.write(buf);
			
			// if(out)
			// 	socket.write(out);
			// socket.destroy();		
		});

		socket.on("end", () => {
			console.log("Client disconnected");
		});

		socket.on("error", (error) => {
			console.log(`Socket Error: ${error.message}`);
		});
	});

	server.on("error", (error) => {
		console.log(`Server Error: ${error.message}`);
	});

	server.listen(port, () => {
		cl('Listening on port ' + port);
	});
}

(async()=>{
	try{
		init();
		await getBotUpdates();
	}catch(e)
	{
		cl("ERROR: ",e);
	}
	//cl(await botCall('getMe'));
	
})()

