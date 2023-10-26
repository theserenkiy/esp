const port = 8124;
process.title = "BINARY SERVER @ "+port;
const cl = console.log;

import net from 'net';
import fs from 'fs';

const token_users = {
	"W48H4TKMBJPX6B5": "Sergey",
	"X29RN28N8FGSCBA": "Temo4ka",
	"597R3Q94V2T37FS": "Evgeniy",
	"7EG45HARJAYBQTH": "Nill Kiggers",
	"58B4SYCGSV5Q3YS": "Jilling Kews",
	"5RGQ4K62APQEXQP": "Sasha",
	"Q5N66F6GNKNH738": "Gleb",
	"H44284GRBPTCE3N": "Vika"
}

const tokens = [
	"X29RN28N8FGSCBA",
	"597R3Q94V2T37FS",
	"7EG45HARJAYBQTH",
	"58B4SYCGSV5Q3YS",
	"5RGQ4K62APQEXQP",
	"Q5N66F6GNKNH738",
	"H44284GRBPTCE3N",
	"75WS2VYD9FBB76Z",
	"N26UZ67SBE9J64W",
	"W48H4TKMBJPX6B5"
]

function ntString(buf, start, end)
{
	let subbuf = buf.slice(start,end)
	let nulpos = subbuf.indexOf(0)
	return subbuf.slice(0,nulpos).toString('utf-8');
}


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

				if(!tokens.includes(token))
					throw "Invalid token";

				username = token_users[token] || 'Noname'
				cmd = data.readUInt32LE(16);
				payload_len = data.readUInt32LE(20);
				totaldata = Buffer.alloc();

				console.log(`Received cmd: ${cmd}, payload len: ${payload_len}`);

			}
			else{
				totaldata = tokens
			}

			if(totaldata.length < payload_len+24)
				return;

			let payload = data.slice(24, 24+payload_len);
			
			switch(cmd)
			{
				case 1:
					out = "Privet, "+username+"!";
					break;
				
				case 2:
					let to = ntString(payload,0,16);
					
					fs.appendFileSync('chat.txt', JSON.stringify({
							from: username,
							to,
							msg: ntString(payload,16,payload_len),
							time: Date.now()
						})+'\n'
					)
					break;
				
				case 3:
					let lines_count = data.readUInt32LE(24);
					let lines = fs.readFileSync('chat.txt','utf-8').split('\n');
					out = lines.slice(-lines_count).filter(v=>v.trim()).map(json => {
						let v = JSON.parse(json);
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

