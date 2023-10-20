const port = 1337;
process.title = "BINARY CLIENT @ "+port;



var net = require('net');

var client = new net.Socket();
client.connect(port, '127.0.0.1', function() {
	console.log('Connected');
    let buf = Buffer.alloc(4);
    buf.writeUInt32LE(1234);
    client.write(buf);
});

client.on('data', function(data) {
	console.log('Received: ',data, data.toString());
	//client.destroy(); // kill client after server's response
});

client.on('close', function() {
	console.log('Connection closed');
});