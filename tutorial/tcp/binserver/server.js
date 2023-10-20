const port = 1337;
process.title = "BINARY SERVER @ "+port;
const cl = console.log;

var net = require('net');

const tokens = [
	"X29RN28N8FGSCBAD",
	"597R3Q94V2T37FSN",
	"7EG45HARJAYBQTHF",
	"58B4SYCGSV5Q3YSZ",
	"5RGQ4K62APQEXQPT",
	"Q5N66F6GNKNH738G",
	"H44284GRBPTCE3NB",
	"75WS2VYD9FBB76ZD",
	"N26UZ67SBE9J64WU",
	"W48H4TKMBJPX6B5W"
]

const types = {
	u32: ['readUInt32LE', 4],
	s32: ['readInt32LE', 4],
	u8: ['readUInt8LE', 1],
	s8: ['readInt8LE', 1],
}

function parseStruct(buf,struct)
{
	let data = {}
	let offset = 0;
	for(let field in struct)
	{
		let type = struct[field]
		if(types[type])
		{
			let [foo,len] = types[type][0];
			data[field] = buf[foo](offset);
			offset += len;
		}
		else
		{

		}
	}
}

const server = net.createServer((socket) => {
    console.log("Client connected");

    socket.on("data", (data) => {
		let out = "";
		let status = 0;
		console.log(`Received data`, data);
		try{
			let token = data.toString('utf-8',0,16);

			if(!tokens.includes(token))
				throw "Invalid token";

			let cmd = data.readUInt32BE(16);
			console.log(`Received cmd: ${cmd}`);
			
			let out;
			switch(cmd)
			{
				case 1:
					out = "Privet, "+token;
					break;
			}
		}
		catch(e)
		{
			cl(e)
			status = 16;
			out = (e+'').substring(0,16);
		}
		let buf = Buffer.alloc(8);
		buf.writeUInt32LE(status);
		buf.writeUInt32LE(out.length);
		socket.write(buf);
		socket.write(out);
		socket.close();
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

