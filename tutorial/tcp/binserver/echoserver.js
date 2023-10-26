const port = 8640;
process.title = "BINARY ECHO SERVER @ "+port;
const cl = console.log;

const net = require('net');
const fs = require('fs');
const server = net.createServer((socket) => {
    console.log("Client connected");

    socket.on("data", (data) => {
        try{
            socket.write(data);
		}
		catch(e)
		{
			cl(e)
		}
		socket.destroy();		
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

