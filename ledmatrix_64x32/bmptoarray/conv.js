const bmp = require('bmp-js')
const fs = require('fs')

var bmpBuffer = fs.readFileSync('test.bmp');
var bmpData = bmp.decode(bmpBuffer);

let outdata = []
let d = bmpData.data
for(let i=0; i < d.length;i+=4)
{
    //outdata.push(((d[i+1]>>6)) | ((d[i+2]>>5) << 2) | ((d[i+3]>>5) << 5));
    outdata.push(d[i+3]);
    outdata.push(d[i+2]);
    outdata.push(d[i+1]);
}

//console.log(bmpData)
console.log('unsigned char image[] = {'+outdata.map(v => '0x'+v.toString(16).padStart(2,'0')).join(',')+'};');

