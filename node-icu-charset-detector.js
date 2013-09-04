var CharsetMatch = require("./build/Release/node-icu-charset-detector").CharsetMatch;

function detectCharset(buffer, options) {
  var charsetMatch = new CharsetMatch(buffer);

  if (charsetMatch.getName() === null) {
    return null;
  }

  var charset = new String(charsetMatch.getName());
  charset.language = charsetMatch.getLanguage();
  charset.confidence = charsetMatch.getConfidence();

  return charset;
}

function detectCharsetStream(stream, onDetectionFinish) {
  var buffer = null;
  var finished = false;

  function onChunkArrives(chunk) {
    buffer = buffer ? Buffer.concat(buffer, chunk) : chunk;

    var charset = detectCharset(buffer);
    if (charset !== null) {
      // detection succeed. stop reading chunks.
      stream.removeListener("data", onChunkArrives);
      finished = true;
      onDetectionFinish(charset);
    }
  }

  stream.on("data", onChunkArrives);
  stream.on("end", function () {
    if (!finished) {
      onDetectionFinish(null);
    }
  });
}

exports.detectCharset = detectCharset;
exports.detectCharsetStream = detectCharsetStream;
exports.CharsetMatch = CharsetMatch;
