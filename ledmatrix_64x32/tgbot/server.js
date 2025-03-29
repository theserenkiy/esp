const port = 8124;
process.title = "BINARY SERVER @ "+port;

const cl = console.log;

import net from 'net';
import fs from 'fs';
import fetch from 'node-fetch';
import botlib from './botlib.js'
import lib from './lib.js'
import sharp from 'sharp'
import { HttpsProxyAgent } from 'https-proxy-agent';

const bot_token = "bot8163000125:AAFkZpGtwKhd_Y4VwyhFHssAnhLP3WA9pR4";
const proxy = '192.168.88.1:1818';

let media = [];
let max_id = 0;



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
						let last_update = botlib.getUpdateId();
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
		while(1)
		{
			//init();
			let photos = await botlib.getUpdates();
			if(photos)
			{
				let p = photos[photos.length-1]

				let path = await botlib.getFile(p.file_id);
				cl(path);
				let buf = await sharp(path)
					.resize({width:64,height:32})
					.raw()
					.toBuffer()

				fs.writeFileSync('files/curpict.raw',buf);
				fs.unlinkSync(path)
			}
			
			await lib.delay(3000)
		}
		
	}catch(e)
	{
		cl("ERROR: ",e);
	}
	//cl(await botCall('getMe'));
	
})()

