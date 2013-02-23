var CharsetMatch = require("./build/Release/node-icu-charset-detector").CharsetMatch;

function detectCharset(buffer, options) {
  var charsetMatch = new CharsetMatch(buffer);

  var charset = new String(charsetMatch.getName());
  charset.language = charsetMatch.getLanguage();
  charset.confidence = charsetMatch.getConfidence();

  return charset;
}

exports.detectCharset = detectCharset;
exports.CharsetMatch = CharsetMatch;
