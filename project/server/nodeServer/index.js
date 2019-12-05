var https = require("https");
var fs = require('fs');
var express = require('express');
var dgram = require('dgram');
var aesjs = require('aes-js');

var data = [];

var key = [116, 104, 105, 115, 105, 115, 97, 115, 101, 99, 114, 101, 116, 107, 101, 121];

var aesEcb = new aesjs.ModeOfOperation.ecb(key);
function decrypt(mess){

  var decryptedBytes = aesEcb.decrypt(mess);

  // Convert our bytes back into text
  var decryptedText = aesjs.utils.utf8.fromBytes(decryptedBytes);
  //console.log(decryptedText);
  return decryptedText;
}

function toHexString(byteArray) {
  return Array.prototype.map.call(byteArray, function(byte) {
    return ('0' + (byte & 0xFF).toString(16)).slice(-2);
  }).join('');
}


//UDP
var serverUDP = dgram.createSocket('udp6');
serverUDP.on('message', function(message, remote) {
  console.log(remote.address + ' - ' + remote.port);
  var arrInt = Uint8Array.from(message).slice(0,16);
  if(arrInt.length==16){
    var mess = decrypt(arrInt);
    //console.log(mess);
    data.push(mess.replace(/[f]+/g,""));
  }
  else{
    //console.log("error");
  }
});
serverUDP.bind(5678);

//HTTP
var app = express();
app.use(express.static('public'));
app.get('/', function(req, res){
  res.sendFile('index.html');
});
app.get('/data', function(req, res){
  res.send(data.join("|"));
  data=[];
});
/*app.listen(8080, function () {
  console.log('listening on port 8080');
});*/

https.createServer({
  key: fs.readFileSync('key.pem'),
  cert: fs.readFileSync('cert.pem')
}, app)
.listen(8080, function () {
  console.log('listening on port 8080')
})
