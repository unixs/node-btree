# node-btree

[![NPM](https://nodei.co/npm/node-btree.png?compact=true)](https://nodei.co/npm/node-btree/)

[![Build Status](https://travis-ci.org/unixs/node-btree.svg?branch=master)](https://travis-ci.org/unixs/node-btree)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
[![GitHub issues](https://github.com/unixs/node-btree/issues)](https://img.shields.io/github/issues-raw/unixs/node-btree)


## Brief

Node.js native Balanced Binary Tree implementation based on [GTree from GLib](https://developer.gnome.org/glib/stable/glib-Balanced-Binary-Trees.html#g-tree-remove).

Writen on C with [N-API](https://nodejs.org/dist/latest-v12.x/docs/api/n-api.html).

Module implement Map interface: `set()`, `get()`, `has()`, etc.
Has one additional property: `height` that store bTree height, and natively support map/reduce operations.

## Latest release changes

See: [CHANGELOG.md](https://github.com/unixs/node-btree/blob/master/CHANGELOG.md)

## Limitations

* Same as GTree:

> The tree may not be modified while iterating over it (you can't add/remove items).

* Node.js >= 10.

* Support building on POSIX platforms only at this moment.

## Dependency & build

For successfull instalation you must have installed libglib binary & C headers.

On Ubuntu GNU/Linux this packages must be installed:

* libglib2.0-dev

## Usage

```js
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

```
