node-postal
-----------

These are the NodeJS bindings to https://github.com/openvenues/libpostal

We're supporting NodeJS bindings primarily because Pelias (https://github.com/pelias) is written in Node.

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
