node-postal
-----------

These are the NodeJS bindings to https://github.com/openvenues/libpostal, a fast, multilingual NLP library for parsing/normalizing physical addresses. libpostal is written in C because virtually every other language can bind to C.

We're supporting official NodeJS mostly because Pelias (https://github.com/pelias), Mapzen's geocoder, is written in Node, but also because it's an awesome, ubiquitous language that's immensely popular among folks in the geo world.

Installation
------------

Before using the Node bindings, you must install libpostal first. Make sure you have the following prerequisites:

**On Linux (Ubuntu)**
```
sudo apt-get install libsnappy-dev autoconf automake libtool
```

**On Mac OSX**
```
sudo brew install snappy autoconf automake libtool
```

**Installing libpostal**

```
git clone https://github.com/openvenues/libpostal
cd libpostal
./bootstrap.sh
./configure --datadir=[...some dir with a few GB of space...]
make
sudo make install

# On Linux it's probably a good idea to run
sudo ldconfig
```

**Installing node-gyp on Linux (Ubuntu)**

First make sure you have:

1. gcc, g++ and make (```sudo apt-get install build-essential``` on Ubuntu)
2. NodeJS with all the development headers (if using a package manager like apt-get, follow the instructions here: https://nodejs.org/en/download/package-manager/)
3. python 2.7

*On Mac OSX*
```
cd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr
sudo ln -s /usr/local local
```

Then install node-gyp system-wide:

```
sudo npm install -g node-gyp
```

**Installing the Node bindings**

```
npm install openvenues/node-postal
```

Usage
-----

```node
var postal = require('node-postal');

// Expansion API
postal.expand.expand_address('V XX Settembre, 20', {languages: ['it']})

// Parser API
postal.parser.parse_address('Mayfield 688 Franklin Ave, Crown Heights, Brooklyn, NY 11238')
```

Compatibility
-------------

The C++ bindings are written using Nan (https://github.com/nodejs/nan), which abstracts many of the API changes across versions of V8. That should keep node-postal compatible

If you're having trouble compiling, post a Github issue with your OS and Node versions along with the output of npm install / node-gyp rebuild.

Tests
-----

To run the unit tests:

```
npm test
```

Special Thanks
--------------

Lucas Hrabovsky (https://github.com/imlucas) for information and examples of repos using Nan, node-gyp, etc.
