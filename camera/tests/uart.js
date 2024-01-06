const cl = console.log;
const { SerialPort } = require('serialport')
const fs = require('fs');
const port = new SerialPort({
  path: 'COM4',
  baudRate: 115200,
  autoOpen: false,
})

port.open(function (err) {
	if (err) {
	return console.log('Error opening port: ', err.message)
	}

	// Because there's no callback to write, write errors will be emitted on the port:
	//port.write('main screen turn on')
	console.log('Opened...')
})

// The open event is always emitted
port.on('open', function() {
	console.log('On open...')
  // open logic
})

let frameStarted = 0;
function addLine(line)
{
	let line_bytes = new Uint8Array(315);
	for(let i=0;i < 315; i++)
	{
		line_bytes[i] = parseInt(line.substring(i*2,i*2+2),16);
	}
	if(frameStarted)
		frame.push(line_bytes);
	//console.log(bytes);
}

function flushFrame()
{
	if(!frameStarted)
	{
		frameStarted = 1;
		return;
	}
	let p;
	for(let line=0; line < 240; line+=2)
	{
		let s = '';
		for(let pix=0; pix < 315; pix+=2)
		{
			p = frame[line][pix];
			s += p < 8 ? ' ' : (p < 16 ? '.': (p < 24 ? 'o' : '0'))
		}
		console.log(s);
	}
	console.log('\n------------------------------------------------------------------------------\n');
	frame = [];
}

let frame_id = 0;
port.on('data',d => {
	if(frame_id)
	{
		fs.appendFileSync('frames/'+frame_id+'.bin',d);
	}
	if(d[d.length-1]==0xff)
	{
		cl('Frame ended');
		if(frame_id)
			fs.writeFileSync('frame_id.json','{"frame_id":'+frame_id+'}');
		frame_id = Date.now();
	}
})

let line = '';
let nl_found = 0;
// port.on('data',d => {

// 	// console.log('>',d);
// 	// d += ''
// 	// console.log('>'+d);
// 	// return;
// 	if(nl_found && d[0]==' ')
// 	{
// 		flushFrame();
// 		d = d.substring(1,d.length);
// 		nl_found = 0;
// 	}
// 	let nl = d.indexOf(' ');
// 	if(nl >= 0)
// 	{
// 		d+= ''
// 		line += d.substring(0,nl);
// 		addLine(line);
// 		nl_found = 1;
// 		if(d[nl+1]==' ')
// 			flushFrame();
// 		nl_found = 0;
// 		line = d.substring(nl+1,d.length);
// 	}
// 	else line += d;
// })

