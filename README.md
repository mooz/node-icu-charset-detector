# ICU Character Set Detection for Node.js

>Character set detection is the process of determining the character set, or encoding, of character data in an unknown format.

A simple binding of ICU character set detection (http://userguide.icu-project.org/conversion/detection) for Node.js.

## Installation

At first, install `libicu` into your system (See [this instruction](#installing-icu) for details).

After that, install `node-icu-charset-detector` from npm.

    ```npm install node-icu-charset-detector```

### Installing ICU

#### Linux

* Debian (Ubuntu)

    ```apt-get install libicu-dev```

* Gentoo

    ```emerge icu```

#### OSX

* MacPorts

    ```port install icu +devel```

* Homebrew

    ```shell
    brew install icu4c
    ln -s /usr/local/Cellar/icu4c/<VERSION>/bin/icu-config /usr/local/bin/icu-config
    ln -s /usr/local/Cellar/icu4c/<VERSION>/include/unicode /usr/local/include
    ```

If experiencing issues with 'homebrew' installing version 50.1 of icu4c, try the following:

    ```shell
    brew search icu4c
    brew tap homebrew/versions
    brew versions icu4c
    cd $(brew --prefix) && git pull --rebase
    git checkout c25fd2f $(brew --prefix)/Library/Formula/icu4c.rb
    brew install icu4c
    ```

## Usage

### Simple usage

`node-icu-charset-detector` provides a function `detectCharset(buffer)`, where `buffer` is an instance of `Buffer` whose charset should be detected.

```javascript
var charsetDetector = require("node-icu-charset-detector");

var buffer = fs.readFileSync("/path/to/the/file");
var charset = charsetDetector.detectCharset(buffer);

console.log("charset name: " + charset.toString());
console.log("language: " + charset.language);
console.log("detection confidence: " + charset.confidence);
```

`detectCharset(buffer)` returns the detected charset name for `buffer`, and the returned charset name has two extra properties `language` and `confidence`:

- `charset.language`
  - language name for the detected character set.
- `charset.confidence`
  - confidence of the charset detection for `charset`.

### Leveraging node-iconv

Since ICU itself does not have a feature to convert character sets, you may need to use `node-iconv` (https://github.com/bnoordhuis/node-iconv), which has a powerful character sets converting feature.

Here is a simple example to leverage `node-iconv` to convert character sets not supported by Node itself.

```javascript
function bufferToString(buffer) {
  var charsetDetector = require("node-icu-charset-detector");
  var charset = charsetDetector.detectCharset(buffer).toString();

  try {
    return buffer.toString(charset);
  } catch (x) {
    var Iconv = require("iconv").Iconv;
    var charsetConverter = new Iconv(charset, "utf8");
    return charsetConverter.convert(buffer).toString();
  }
}

var buffer = fs.readFileSync("/path/to/the/file");
var bufferString = bufferToString(buffer);
```
