# node-btree

[![Build Status](https://travis-ci.org/unixs/node-btree.svg?branch=master)](https://travis-ci.org/unixs/node-btree)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)

![GitHub issues](https://img.shields.io/github/issues-raw/unixs/node-btree)
![npm](https://img.shields.io/npm/v/node-btree)

## Brief

Node.js native Balanced Binary Tree implementation based on [GTree from GLib](https://developer.gnome.org/glib/stable/glib-Balanced-Binary-Trees.html#g-tree-remove).

Writen on C with [N-API](https://nodejs.org/dist/latest-v12.x/docs/api/n-api.html).

Module implement Map interface: `set()`, `get()`, `has()`, etc.
And has one additional property: `height` that store bTree height.

## Limitations

Same as GTree:

`
The tree may not be modified while iterating over it (you can't add/remove items). To remove all items matching a predicate, you need to add each item to a list in your GTraverseFunc as you walk over the tree, then walk the list and remove each item.
`

So.. for-of cycle is work fine, but usage forEach() method - preferred.

Node.js >= 10.

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


btree.forEach((key, value) => {
  console.log(key, value);
});
// "15" 150
// "30" 30
// "50" 50

btree.size;
// 3

btree.height;
// 2

```
