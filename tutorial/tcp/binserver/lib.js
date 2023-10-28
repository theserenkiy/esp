import fs from 'fs';
import path from 'path';
const tokens = JSON.parse(fs.readFileSync('./tokens.json', 'utf8'))
const token_users = JSON.parse(fs.readFileSync('./token_users.json', 'utf8'))

const root = path.resolve('.');
const cl = console.log;
export function getMessages(to,fromtime,limit)
{
	let mm = fs.readFileSync(root+'/chat.txt', 'utf8')
		.trim().split('\n').map(s => JSON.parse(s)).filter(d => d.to == to);

	if(fromtime)
		mm = mm.filter(d => d.time > fromtime);
	if(limit)
		mm = mm.slice(-limit);
	return mm;
}

export function getUserLastReadTime(token)
{
	let d = JSON.parse(fs.readFileSync(root+'/readtimes.json', 'utf8'));
	let time = d[token] || 0;
	d[token] = Date.now();
	fs.writeFileSync(root+'/readtimes.json',JSON.stringify(d), 'utf8');
	return d[token];
}

export function ntString(buf, start, end)
{
	let subbuf = buf.subarray(start,end)
	let nulpos = subbuf.indexOf(0)
	return subbuf.subarray(0,nulpos).toString('utf-8');
}

export function getUserToken(username)
{
	username = username.trim().toLowerCase();
	if(username=='all')
		return 1;
	cl('USERS ',Object.entries(token_users))
	cl('USERNAME ',username)
	let res = Object.entries(token_users).find(v => v[1].toLowerCase()==username);
	if(!res)
		throw "User not found: " + username;
	return res[0];
}

export function getTokenUsername(token)
{
	return token_users[token] || "Anonymous";
}

export function checkToken(token)
{
	return tokens.includes(token);
}