import lib from './lib.js'
import bot from './botlib.js'


(async ()=>{

	while(true)
	{
		await bot.getUpdates();
		await lib.delay(2000);
	}
	
})()
