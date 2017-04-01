node-postal
-----------

[![Build Status](https://travis-ci.org/openvenues/node-postal.svg?branch=master)](https://travis-ci.org/openvenues/node-postal) [![npm](https://img.shields.io/npm/v/node-postal.svg?maxAge=2592000)](https://www.npmjs.com/package/node-postal)

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
sudo apt-get install curl autoconf automake libtool pkg-config
```

**On CentOS/RHEL**
```
sudo yum install curl autoconf automake libtool pkgconfig
```

**On Mac OSX**
```
brew install curl autoconf automake libtool pkg-config
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
**Install node-gyp**

Then install node-gyp system-wide:

```
sudo npm install -g node-gyp
```

**Installing node-gyp on Linux (Ubuntu)**

First make sure you have:

1. gcc, g++ and make (```sudo apt-get install build-essential``` on Ubuntu)
2. NodeJS with all the development headers (if using a package manager like apt-get, follow the instructions here: https://nodejs.org/en/download/package-manager/)
3. python 2.7


**Installing the Node bindings**

```
npm install openvenues/node-postal
```

Compatibility
-------------

node-postal is tested on most major versions of Node (0.10+) in continuous integration. The C++ bindings are written using [Nan](https://github.com/nodejs/nan), which abstracts the API changes across various versions of V8, so the latest versions of Node will likely work.

If you're having trouble compiling, post a Github issue with your OS and Node versions along with the output of npm install / node-gyp rebuild.

Tests
-----

To run the unit tests:

```
npm test
```

Troubleshooting
---------------
**Installing [libpostal](https://github.com/openvenues/libpostal):**
node-postal is a thin wrapper around libpostal, which is written in C. Installing node-postal with npm will not install libpostal as a dependency. The install steps described above will install it in standard locations for your OS. For OS X and Linux, that's `/usr/local/include` and `/usr/local/lib`. node-postal will look in these locations for the C libraries. If you have problems installing node-postal, check there for recently installed libpostal libraries.

**Downloading Large Files**
The ```make``` process downloads ~3GB of data (these are production-quality machine learning models, the kind Google engineers get to use). Make sure the --datadir argument to ```configure```, which requires an absolute path, has enough room to accommodate the models. If you get errors after downloading, e.g.:
```
make[3]: *** [all-local] Error 1
make[2]: *** [all-recursive] Error 1
make[1]: *** [all-recursive] Error 1
make: *** [all] Error 2
```

there may be problems with subsequent install steps. Occasionally something will go wrong and it will give up downloading. Once downloading is complete, the make script will unpack the langauge_classifier.tar.gz.X files. If you see these files, your download may have stopped. Try running the make script again.

Special Thanks
--------------

Lucas Hrabovsky (https://github.com/imlucas) for information and examples of repos using Nan, node-gyp, etc.
