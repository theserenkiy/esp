const fs = require('fs');	
const bmp = require('bmp-js');

const bdata = fs.readFileSync('img1.bmp')

let mask = 0xff;
for(let bits = 8; bits > 0; bits--) {
	for(let i=40; i < bdata.length; i++)
	{
		bdata[i] &= mask;
	}
	fs.writeFileSync('img1_'+bits+'bit.bmp', bdata);
	mask = mask << 1;
}
//let encoded = bmp.encode(bdata);
//console.log(encoded)
