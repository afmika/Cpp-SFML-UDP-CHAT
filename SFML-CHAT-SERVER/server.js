var udp = require('dgram');
var buffer = require('buffer');

var server = udp.createSocket('udp4');
const port_binded = 6666;

const users_port_set = new Set();
const ip_adress = 'localhost';

// emits when any error occurs
server.on('error', function(error){
	console.log('Error: ' + error);
});

server.on('message',function(senderMsg, senderInfos){
	console.log('Data received from client : ' + senderMsg.toString());
	console.log('Received %d bytes from %s:%d\n', senderMsg.length, senderInfos.address, senderInfos.port);
	
	users_port_set.add( senderInfos.port );
	console.log( users_port_set );
	
	senderMsg = senderMsg+'';
	if ( senderMsg.includes('CHECK_CONNECTION') ) {
		server.send(Buffer.from( "CONNECTION OK BRUH!" ), senderInfos.port, ip_adress, function(error){
			if(error){
				console.log( error );
			}else{
				console.log('Data sent to everyone !!!');
			}
		});		
	} else {
		senderMsg = senderMsg.split("|");
		senderMsg = "["+ senderMsg[0] + "]  " + senderMsg[1];
		users_port_set.forEach( user_port => {		
			//sending senderMsg
			server.send(Buffer.from( senderMsg ), user_port, ip_adress, function(error){
				if(error){
					console.log( error );
				}else{
					console.log('Data sent to everyone !!!');
				}
			});
		});
	}
});

//emits when socket is ready and listening for datagram senderMsgs
server.on('listening',function(){
	var address = server.address();
	var port = address.port;
	var family = address.family;
	var ipaddr = address.address;
	console.log('Server is listening at port' + port);
	console.log('Server ip :' + ipaddr);
	console.log('Server is IP4/IP6 : ' + family);
});

//emits after the socket is closed using socket.close();
server.on('close',function(){
	console.log('Socket is closed !');
});

server.bind( port_binded );