const port = 8124;
process.title = "BINARY SERVER @ "+port;

const cl = console.log;

import net from 'net';
import fs from 'fs';
import { exec } from 'child_process';
import fetch from 'node-fetch';
import botlib from './botlib.js'
import lib from './lib.js'
import sharp from 'sharp'
import { HttpsProxyAgent } from 'https-proxy-agent';

const bot_token = "bot8163000125:AAFkZpGtwKhd_Y4VwyhFHssAnhLP3WA9pR4";
const proxy = '192.168.88.1:1818';

let dummy = 0//502255185;

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

async function savePhoto(d){
	let buf = await sharp(d.path)
		.resize({width:64,height:32})
		.raw()
		.toBuffer()

	fs.writeFileSync('files/curpict.raw',buf);
	//fs.unlinkSync(path)
	lib.setVar('current',d)
	botlib.send(d.from, `Изображение обработано!`)
}

async function saveVideo(d) {
	cl('Save video',d)	
	let dir = 'files/'+d.fbname
	fs.rmSync(dir,{recursive:true, force:true})
	let ffdir = dir+'/ffmpeg'
	let shdir = dir+'/sharp'
	fs.mkdirSync(dir);
	fs.mkdirSync(ffdir);
	fs.mkdirSync(shdir);

	let cmd = `ffmpeg -i ${d.path} -filter:v scale=128:-1,fps=15 ${ffdir}/frame%04d.png`

	try{
		await new Promise((s,j) => {
			exec(cmd,(err,stdout,stderr) => {
				if(err)j(err);
				cl({stdout});
				cl({stderr});
				s()
			})
		})
		if(!fs.existsSync(ffdir+'/frame0001.png'))
			throw "No frames exported"

		cl('Converting frames...')
		let ff = fs.readdirSync(ffdir);
		d.frames = ff.length;

		for(let f of ff)
		{
			cl('sharp',f)
			let buf = await sharp(ffdir+'/'+f)
				.resize({width:64, height: 32})
				.raw()
				.toBuffer()
			
			fs.appendFileSync(shdir+'/common.raw',buf)
		}
		fs.copyFileSync(shdir+'/common.raw','files/current.raw')
		lib.setVar('current',d)
		if(!dummy)
			botlib.send(d.from, `Видео обработано! Количество кадров: ${d.frames}`)
		fs.rmSync(dir,{recursive:true, force:true})

	}catch(e)
	{
		cl(e)
		if(!dummy)
			botlib.send(d.from, "Произошла ошибка при обработке видео (")
	}
}

if(dummy)
	botlib.dummy_sends = 1;

(async()=>{
	try{
		while(1)
		{
			let upd = await botlib.checkUpdates(dummy);
			if(upd)
			{
				if(upd.is_video && !dummy)
					botlib.send(upd.from, "Видео обрабатывается...")

				let fdata = await botlib.getFile(upd.file_id);
				fdata.from = upd.from;
				fdata.is_video = upd.is_video;
				if(upd.is_video)
					await saveVideo(fdata)
				else
					await savePhoto(fdata)
			}
			
			if(dummy)
				break;
			await lib.delay(3000)
		}
		
	}catch(e)
	{
		cl("ERROR: ",e);
	}
	//cl(await botCall('getMe'));
	
})()

