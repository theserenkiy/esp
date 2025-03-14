const cl = console.log;

import net from 'net';
import fs from 'fs';
import fetch from 'node-fetch';
import lib from './lib.js';
import { HttpsProxyAgent } from 'https-proxy-agent';

import User from './user.js'

let bot_token = lib.getVar('botkey');
let last_id = lib.getVar('last_id');
const proxy = '192.168.88.1:1818';

const binary_server = {
	ip: '37.46.135.97',
	port: 4984
}


async function callMethod(method,params={})
{
	let param_str = Object.entries(params).map(v => v[0]+'='+encodeURIComponent(v[1])).join('&');
	if(param_str)param_str = '?'+param_str;
	let url = 'https://api.telegram.org/bot'+bot_token+'/'+method+param_str;
	cl('BOT CALL:',url)
	let res = await fetch(url, proxy ? {agent: new HttpsProxyAgent('http://'+proxy)} : {});
	let d = await res.json();
	if(!d.ok)
		throw "TG API ok=false @"+method;
	return d;
}


async function getUpdates()
{
	let id = lib.getIntVar('last_id');

	let d = await callMethod('getUpdates',{offset:id ? +id : -10});
	cl(d)
	for(let res of d.result)
	{
		if(res.update_id <= id)
			continue;
		id = res.update_id;

		parseMessage(res.message)

		//cl(JSON.stringify(res,null,'	'));
    }
	lib.setVar('last_id',id);
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
		return send(user, `Invalid command`);
	if(!cmds[cmd])
		return send(user, `Unknown command "${cmd}"`);

	let res;
	if(Array.isArray(cmds[cmd]))
	{
		let curcmd = {cmd,step:0};
		continueCmd(user,text,curcmd);
	}
	else
		send(user,cmds[cmd](user,text));
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
	send(user,res);
}


function parseMessage(m)
{
	let user = new User(m.from);
	user.parseMsg(m);
}

function send(user,msg)
{
	return botCall('sendMessage',{chat_id: user.id, text: msg});
}


export default {
	getUpdates,
	callMethod
}
