var aesjs = require('aes-js');

var key = [116, 104, 105, 115, 105, 115, 97, 115, 101, 99, 117, 114, 101, 107, 101, 121];

// Convert text to bytes
var text = 'A,10234fffffffff';
var textBytes = aesjs.utils.utf8.toBytes(text);

var aesEcb = new aesjs.ModeOfOperation.ecb(key);
var encryptedBytes = aesEcb.encrypt(textBytes);

// To print or store the binary data, you may convert it to hex
var encryptedHex = aesjs.utils.hex.fromBytes(encryptedBytes);
console.log(encryptedHex);
// "a7d93b35368519fac347498dec18b458"

// When ready to decrypt the hex string, convert it back to bytes
var encryptedBytes = aesjs.utils.hex.toBytes(encryptedHex);

// Since electronic codebook does not store state, we can
// reuse the same instance.
//var aesEcb = new aesjs.ModeOfOperation.ecb(key);
var decryptedBytes = aesEcb.decrypt(encryptedBytes);

// Convert our bytes back into text
var decryptedText = aesjs.utils.utf8.fromBytes(decryptedBytes);
console.log(decryptedText);
// "TextMustBe16Byte"
