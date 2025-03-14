import cmds from './cmds.js'
import bot from './botlib.js'

export default class Command
{
	name = ''
	step = 0
	cmdpack = null
	cmd = null
	result = 'ok'
	data = {}

	static getByName(name,user)
	{
		if(!name)
			return this.send(`Неправильная команда`);
		if(!cmds.logic[name])
			this.send(`Неизвестная команда "${name}"`);
		
		return new Command(name,user)
	}

	static getBySaved(saved,user)
	{
		if(saved && saved.name)
		{
			let c = Command.getByName(saved.name,user)
			c.load(saved)
			return
		}
	}

	constructor(name,user)
	{
		let cmd = cmds.logic[name]
		this.cmdpack = Array.isArray(cmd) ? cmd : [cmd];
		this.user = user;
	}

	load(obj)
	{
		Object.assign(this,obj)
	}

	run(txt)
	{
		this.text = txt;
		this.cmdpack[this.step](this)
		if(this.result == 'ok')
		{
			this.step++;
			if(this.cmdpack.length >= this.step)
			{
				this.terminate()
			}
			else
			{
				this.save()
			}
		}
		else 
		{
			this.save()
		}
	}

	save()
	{
		this.user.curcmd = {name: this.name, step: this.step, data: this.data};
	}

	terminate()
	{
		this.user.curcmd = null;
	}
	
	dataError(msg)
	{
		this.result = 'data_error';
		this.resp(msg)
	}

	resp(msg)
	{
		bot.callMethod('sendMessage',{chat_id: this.user.id, text: msg});
	}
}