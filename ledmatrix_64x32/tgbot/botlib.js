const cl = console.log;

import net from 'net';
import fs from 'fs';
import path from 'path';
import fetch from 'node-fetch';
import lib from './lib.js';
import { HttpsProxyAgent } from 'https-proxy-agent';

let dummy_sends = 0;

//import User from './user.js'

let bot_token = lib.getVar('botkey');
let last_id = lib.getVar('last_id');
const proxy = null;//'192.168.88.1:1818';

const binary_server = {
	ip: '37.46.135.97',
	port: 4984
}

async function bfetch(url)
{
	return await fetch(url, proxy ? {agent: new HttpsProxyAgent('http://'+proxy)} : {});
}

async function callMethod(method,params={})
{
	let param_str = Object.entries(params).map(v => v[0]+'='+encodeURIComponent(v[1])).join('&');
	if(param_str)param_str = '?'+param_str;
	let url = 'https://api.telegram.org/bot'+bot_token+'/'+method+param_str;
	cl('BOT CALL:',url)
	let res = await bfetch(url);
	let d = await res.json();
	if(!d.ok)
		throw "TG API ok=false @"+method;
	return d;
}

async function getFile(file_id)
{
	let fdata = await callMethod('getFile',{file_id});
	cl({fdata})
	let url = 'https://api.telegram.org/file/bot'+bot_token+'/'+fdata.result.file_path;
	let res = await bfetch(url);
	let file = path.basename(fdata.result.file_path)
	let fpath = 'files/'+file;
	const fileStream = fs.createWriteStream(fpath);
	await new Promise((resolve, reject) => {
		res.body.pipe(fileStream);
		res.body.on("error", reject);
		fileStream.on("finish", resolve);
	});
	return {
		path: fpath,
		fname: file,
		fbname: file.replace(/\.[^\.]+$/,''),
		ext: path.extname(file).toLowerCase().replace('.','')
	};
}

function getUpdateId()
{
	let id = lib.getVar('last_id');
	cl('update_id',id)
	return id
}

function getUpdObject(res)
{
	let upd = null;
	if(res.message.photo)
	{
		let ph = res.message.photo;
		upd = {
			file_id: ph[ph.length-1].file_id,
			is_video: 0
		};
	}
	if(res.message.video)
	{
		let vid = res.message.video;
		upd = {
			file_id: vid.file_id,
			is_video: 1
		};
	}
	upd.from = res.message.from.id;
	return upd
}

async function checkUpdates(dummy=0)
{
	let log = lib.getVar('botlog') || []
	//cl("log",log)
	if(dummy)
	{
		let res = log.find(v => v.update_id == dummy)
		if(!res)
		{
			cl(`Update #${dummy} is not found in botlog`);
			return null;
		}
		return getUpdObject(res)
	}

	let id = getUpdateId()
	

	let d = await callMethod('getUpdates',{offset:id ? +id : -10});
	cl(d)
	let upd = null;
	for(let res of d.result)
	{
		if(res.update_id <= id)
			continue;
		id = res.update_id;
		log.push(res)

		//parseMessage(res.message)
		let upd_ = getUpdObject(res)
		if(upd_)
			upd = upd_
		cl(JSON.stringify(res,null,'	'));
    }
	lib.setJSONVar('botlog',log)
	lib.setVar('last_id',id);
	return upd;
}


function initUser(from)
{
	let userdir = 'users/'+from.id;
	if(!fs.existsSync(userdir))
	{
		fs.mkdirSync(userdir);
		fs.mkdirSync(userdir+'/vars');
	}

	let user = {
		id: from.id,
		dir: userdir,
		name: from.first_name,
		nick: from.username
	}

	let token = getVar('token',user)
	if(!token)
	{
		token = lib.genCode(16,1);
		setVar('token',token,user)
	}
	user.token = token;

	cl('USER INITED',user)

	return user;
}


function callCmd(user,text)
{
	let cmd = text.substr(1,text.length)
	if(!cmd)
		return send(user.id, `Invalid command`);
	if(!cmds[cmd])
		return send(user.id, `Unknown command "${cmd}"`);

	let res;
	if(Array.isArray(cmds[cmd]))
	{
		let curcmd = {cmd,step:0};
		continueCmd(user,text,curcmd);
	}
	else
		send(user.id,cmds[cmd](user,text));
}

function continueCmd(user,text,curcmd)
{
	let cmd = cmds[curcmd.cmd];
	if(!cmd || !cmd[curcmd.step])
		return "An error occured. Please restart command"

	let res = cmd[curcmd.step](user,text,curcmd)
	curcmd.step++;

	if(cmd.length <= curcmd.step)
		curcmd = null;

	setJSONVar('curcmd',curcmd,user);
	send(user.id,res);
}



function send(userid,msg)
{
	if(dummy_sends)
		cl(`DUMMY SEND to ${userid}: ${msg}`)
	else
	{
		cl(`SEND to ${userid}: ${msg}`)
		return callMethod('sendMessage',{chat_id: userid, text: msg});
	}
		
}


export default {
	checkUpdates,
	callMethod,
	getFile,
	getUpdateId,
	send,
	dummy_sends
}
