const port = 8124;
process.title = "BINARY SERVER @ "+port;
const cl = console.log;

import net from 'net';
import fs from 'fs';
import { getMessages, getUserLastReadTime, ntString, getUserToken, getTokenUsername, checkToken } from './lib.js';

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
			if(!payload_len)
			{
				let token = data.toString('utf-8',0,15);
				cl(token)

				if(!checkToken(token))
					throw "Invalid token";

				username = getTokenUsername(token);
				cmd = data.readUInt32LE(16);
				payload_len = data.readUInt32LE(20);
				totaldata = Buffer.alloc(24+payload_len);
				console.log(`Received cmd: ${cmd}, payload len: ${payload_len}`);
			}

			data.copy(totaldata,total_recved);
			total_recved += data.length;

			if(total_recved < payload_len+24)
				return;

			let payload = totaldata.subarray(24, 24+payload_len);
			
			switch(cmd)
			{
				case 1:
					out = "Privet, "+username+"!";
					break;
				
				case 2:
					let to = ntString(payload,0,16);

					if(!getUserToken(to))
						throw "User not found";
					
					fs.appendFileSync('chat.txt', JSON.stringify({
							from: username,
							to,
							msg: ntString(payload,16,payload_len),
							time: Date.now()
						})+'\n'
					)
					break;
				
				case 3:
					let limit = payload.readUInt32LE(0);					
					let lasttime = getUserLastReadTime(token);			
					let msgs = getMessages(username, lasttime, limit);

					out = msgs.map(v => {
						return v.username+': '+v.msg
					}).join('\n')
					break;
			}
		}
		catch(e)
		{
			cl(e)
			status = 1;
			out = (e+'').substring(0,16);
		}
		let buf = Buffer.alloc(8);
		buf.writeInt32LE(status);
		buf.writeUInt32LE(Buffer.byteLength(out),4);
		socket.write(buf);
		cl('Out: ',out)
		if(out)
			socket.write(out);
		socket.destroy();		
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

