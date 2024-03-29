# node-btree

[![NPM](https://nodei.co/npm/node-btree.png?compact=true)](https://nodei.co/npm/node-btree/)

[![Node.js CI](https://github.com/unixs/node-btree/workflows/Node.js%20CI/badge.svg)](https://github.com/unixs/node-btree/actions)
[![codecov](https://codecov.io/gh/unixs/node-btree/branch/codecov/graph/badge.svg?token=S389BKWNUK)](https://codecov.io/gh/unixs/node-btree)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
[![GitHub issues](https://img.shields.io/github/issues-raw/unixs/node-btree)](https://github.com/unixs/node-btree/issues)

## Brief

Node.js native Balanced Binary Tree bindings to [GTree from GLib](https://docs.gtk.org/glib/struct.Tree.html).

Writen on C with [N-API](https://nodejs.org/dist/latest-v12.x/docs/api/n-api.html).
The module implements Map interface: `set()`, `get()`, `has()`, etc.
It has one additional property: `height` that store bTree height, and natively support map/reduce/filter operations.

## Latest release changes

See: [CHANGELOG.md](https://github.com/unixs/node-btree/blob/master/CHANGELOG.md)

## Documentation

See: [GitHub Wiki](https://github.com/unixs/node-btree/wiki/API)

## Limitations

* **All methods will compiled for GLIB version >= 2.68 only!**

* Same as GTree:

> The tree may not be modified while iterating over it (you can't add/remove items).

* Node.js >= 12.

* Support building on POSIX platforms only at this moment.

## Dependency & build

* [CMake tool](https://cmake.org/).
* pkg-config tool (on POSIX)
* libglib binary & C headers

On Ubuntu GNU/Linux this packages must be installed:

* cmake
* build-essentials
* libglib2.0-dev

On MacOS use **brew** and install:

* cmake
* pkg-config
* glib

## Usage

```js
/**
 * Basic
 */

const { BTree } = require("node-btree");

function comparator(a, b) {
  if (a > b) {
    return 1;
  }
  else if (a < b) {
    return -1;
  }
  else {
    return 0;
  }
}

const btree = new BTree(comparator);

btree.set("50", 50);
btree.set("30", 30);
btree.set("15", 150);

btree.get("15");
// 150
btree.has("30");
// true


btree.forEach((val, key) => {
  console.log(key, val);
});
// "15" 150
// "30" 30
// "50" 50

btree.map((val) => val);
// [150, 30, 50]

btree.reduce((acc, val) => acc + val, 0);
// 230

btree.size;
// 3
btree.height;
// 2

/**
 * From Map()
 */

const map = new Map();

map.set(10, "10");
map.set(30, "30");
map.set(80, "80");
map.set(20, "20");
map.set(50, "50");

const btree2 = BTree.from(comparator, map);

btree.size;
// 5
btree.get(30);
// "30"

/**
 * From iterable
 */

function* generator() {
  for (let i = 0; i < 1000; i++) {
    yield {
      key: i,
      value: `${i}`
    };
  }
}

const btree = BTree.from(comparator, generator());

btree.size;
// 1000
btree.height;
// 10
btree.get(500);
// "500"

```

See more docs here: [GitHub Wiki](https://github.com/unixs/node-btree/wiki/API)
