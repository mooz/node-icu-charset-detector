var NativeCharsetMatch = require("./build/Release/node-icu-charset-detector").CharsetMatch;

function CharsetMatch(buffer) {
  if (!(buffer instanceof Buffer)) {
    throw "Expected an instance of Buffer";
  }
  this.native = new NativeCharsetMatch(buffer);
}

CharsetMatch.prototype = {
  getName: function () {
    return this.native.getName.apply(this.native, arguments);
  },

  getLanguage: function () {
    return this.native.getLanguage.apply(this.native, arguments);
  },

  getConfidence: function () {
    return this.native.getConfidence.apply(this.native, arguments);
  }
};

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

  if (!(stream instanceof require("stream").Stream)) {
    throw "Expected an instance of Stream";
  }

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
