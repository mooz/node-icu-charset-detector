# ICU Character Set Detection for Node.js

>Character set detection is the process of determining the character set, or encoding, of character data in an unknown format.

A simple binding of ICU character set detection (http://userguide.icu-project.org/conversion/detection) for Node.js.

## Installation

At first, install `libicu` into your system. Debian users can install `libicu` by `apt-get` easily.

    sudo apt-get install libicu-dev

After that, install `node-icu-charset-detector` from npm.

    npm install node-icu-charset-detector

If you prefer to install the package by hand, try following commands.

    git clone git://github.com/mooz/node-icu-charset-detector.git
    cd node-icu-charset-detector
    node-waf configure
    node-waf build
    node-waf install

## Usage

### Simple usage

`node-icu-charset-detector` provides a class `CharsetMatch` which takes a instance of `Buffer` for the first argument of the constructor. A instance of `CharsetMatch` has three methods below.

- `CharsetMatch.prototype.getName()`
  - returns the name of detected character set.
- `CharsetMatch.prototype.getLanguage()`
  - returns the language for detected character set.
- `CharsetMatch.prototype.getConfidence()`
  - returns the confidence of detection.

Here is a simple usage of `node-icu-charset-detector`.

    var charsetDetector = require("node-icu-charset-detector");
    var CharsetMatch = charsetDetector.CharsetMatch;
    
    var byteArray = fs.readFileSync(path);
    var charsetMatch = new CharsetMatch(byteArray);
    
    var detectedCharsetName = charsetMatch.getName();
    var detectedLanguage = charsetMatch.getLanguage();
    var detectionConfidence = charsetMatch.getConfidence();

### Leveraging node-iconv

Since ICU itself does not have a feature to convert character sets, you may need to use `node-iconv` (https://github.com/bnoordhuis/node-iconv) which has a powerful character sets converting feature.

Here is a simple example to leverage `node-iconv` to convert character sets which is not supported by native Node.js.

    var Iconv = require("iconv").Iconv;
    
    function bufferToString(buffer, charset) {
    
      try {
        return buffer.toString(charset);
      } catch (x) {
        var charsetConverter = new Iconv(charset, "utf8");
        return charsetConverter.convert(buffer).toString();
      }
    }
    
    var charsetMatch = new CharsetMatch(byteArray);
    var bufferString = bufferToString(byteArray, charsetMatch.getName());
