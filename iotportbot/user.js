import fs from 'fs'
import lib from './lib.js'
import Command from './command.js'

const cl = console.log

export default class User
{
	id = 0
	dir = ''
	varsdir = ''
	token = ''
	nick = ''
	name = ''

	curcmd = null

	vars_to_save = 'id,token,nick,name,curcmd'.split(',')

	constructor(from)
	{
		if(!from)return

		this.initById(from.id)
		this.name = from.first_name
		this.nick = from.username
	
		cl('USER INITED '+this)
	}

	save()
	{
		let d = {}
		for(let i in this.vars_to_save)
		{
			d[i] = this[i];
		}
		lib.setJSONVar('user',d,this)
	}

	read()
	{
		let d = lib.getJSONVar('user',this)
		for(let i in this.vars_to_save)
		{
			this[i] = this[i];
		}
	}

	initById(id)
	{
		this.dir = 'users/'+id;
		this.varsdir = this.dir+'/vars';
		if(!fs.existsSync(this.dir))
		{
			fs.mkdirSync(this.dir);
			fs.mkdirSync(this.varsdir);
		}

		this.read()

		if(!this.token)
		{
			this.token = lib.genCode(16,1);
		}
	}

	toString()
	{
		return `User #${this.id} @${this.nick} ${this.name}`
	}

	parseMsg(m)
	{
		let txt = (m.text || m.caption || '').trim();
		if(txt[0]=='/')
		{
			let cmdname = txt.substr(1,txt.length)
			let cmd = Command.getByName(cmdname,this);
			cmd.run(txt)
		}
		else 
		{
			if(this.curcmd && this.curcmd.name)
			{
				let c = Command.getBySaved(this.curcmd,this)
				c.run(txt)
			}
			else{
				return send(user, `Я не знаю что вам ответить, ${user.name}\nИспользуйте команду /help чтобы понять что тут делать`);
			}
		}
	}

	callCmd(text)
	{
		
	}
}