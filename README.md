node-postal
-----------

[![Build Status](https://travis-ci.org/openvenues/node-postal.svg?branch=master)](https://travis-ci.org/openvenues/node-postal)

These are the NodeJS bindings to [libpostal](https://github.com/openvenues/libpostal), a fast, multilingual NLP library for parsing/normalizing physical addresses. libpostal is written in C because virtually every other language can bind to C.

Usage
-----

```node
var postal = require('node-postal');

// Expansion API
postal.expand.expand_address('V XX Settembre, 20');

// Parser API
postal.parser.parse_address('Mayfield 688 Franklin Ave, Crown Heights, Brooklyn, NY 11238');
```

Installation
------------

Before using the Node bindings, you must install the libpostal C library. Make sure you have the following prerequisites:

**On Ubuntu/Debian**
```
sudo apt-get install curl libsnappy-dev autoconf automake libtool pkg-config
```

**On CentOS/RHEL**
```
sudo yum install snappy snappy-devel autoconf automake libtool pkgconfig
```

**On Mac OSX**
```
brew install snappy autoconf automake libtool pkg-config
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

Then install node-gyp system-wide:

```
sudo npm install -g node-gyp
```

**Installing the Node bindings**

```
npm install openvenues/node-postal
```

Compatibility
-------------

node-postal is tested on most major versions of Node (0.10+) in continuous integration. The C++ bindings are written using [Nan](https://github.com/nodejs/nan), which abstracts the API changes across various versions of V8.

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
