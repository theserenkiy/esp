import 
let cmds = {}

function getCfgString(user)
{
    return `IP: ${binary_server.ip}\nПорт: ${binary_server.port}\nТокен: ${user.token}\n`;
}

function getHelp(user)
{
    let msg = "\nСписок команд бота: \n\n";
    for(let cmd in cmds.descr)
        msg += `/${cmd} - ${cmds.descr[cmd]}\n`

    msg += `\nНастройки API:\n\n${getCfgString(user)}`
    msg += `\nС чего начать:\n\nИспользуйте /addvar чтобы создавать переменные.\nПеременная - это контейнер для данных, отправляемых на устройство, или принимаемых с него.`

    return msg
}


cmds = {
	descr: {
		start: 'Начать работу с ботом',
		help: 'Помощь',
		cfg: 'Посмотреть конфигурацию API (IP и порт сервера, личный токен)',
		token: 'Посмотреть API токен',
		vars: 'Вывести список переменных',
		addvar: 'Добавить новую переменную'
	},

	logic: {
		start: (c) => {
			c.resp(`Привет, ${c.user.name}!\n\n${getHelp(c.user)}`);
		},

		cfg: (c) => {
			c.resp(getCfgString(c.user))
		},

		token: (c) => c.resp(c.user.token),
		help: (c) => c.resp(getHelp(c.user)),
		addvar: [
			c => {
				//"Enter variable config, comma separated fields:\n1. Name: a-z, digits, _\n2. Direction: to/from\n3. Type: onoff | int | text | image | video",
				return c.resp("Введите имя переменной.\nРазрешенные символы: a-z, 0-9, _.\nПеременная не может начинаться с цифры.")
			},
			(c) => {
				if(!/^[a-z_][a-z\d_]*$/.test(c.text))
				{
					return c.dataError("Ошибка. Разрешенные символы: a-z, 0-9, _. Не может начинаться с цифры.\nВведите снова:");
				}
				c.data.varname = text;
				return c.resp("Введите направление: to/from.\nTo = к устройству (например, включить серво или загрузить картинку)\nFrom = от устройства (например, получить значения датчиков)")
			},
			(c) => {
				text = text.toLowerCase();
				if(!/^to|from$/.test(c.text))
				{
					return c.dataError(`Неправильное направление. Введите "to" или "from":`)
				}
				c.data.vardir = text;
				return c.resp("Введите тип данных: onoff | int | text | image | video")
			},
			(user,text,cmd) => {
				if(!/^onoff|int|text|image|video$/.test(c.text))
				{
					return c.dataError(`Неверный тип данных.\nПоддерживаемые типы: onoff, int, text, image, video\nВведите еще раз:`)
				}
				c.data.vartype = text;
				return c.resp("Переменная успешно создана!\nИспользуйте /vars чтобы посмотреть все свои переменные")
			}
		]
	}
		
}

export default cmds;