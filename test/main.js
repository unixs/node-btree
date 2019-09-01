const binary = require('node-pre-gyp');
const path = require('path');
const binding_path = binary.find(path.resolve(path.join(__dirname, '../package.json')));

const ext = require("../lib/binding/Debug/node-v64-linux-x64/node_btree.node");

console.log("node-mixin test.");

console.log(ext.hello());
require('assert')
  .equal(ext.hello(), "hello");

/*
function extend(mixins, save_orig) {
  save_orig = save_orig || false;

  var self = this;

  mixins.forEach(function (mixin) {
    Object.getOwnPropertyNames(mixin.prototype).forEach(function (name) {
      if (name != 'constructor') {
        if (save_orig) {
          self.prototype['__orig_' + name] = self.prototype[name];
        }
        self.prototype[name] = mixin.prototype[name];
      }
    });
  });
}
*/

console.log(BTree);

function comparator(a, b) {
  if (a > b) {
    return +1;
  }
  else if (a < b) {
    return -1;
  }
  else {
    return 0;
  }
}

let tree = new BTree(comparator);

console.log("TREE:" + tree);
console.log("TREE height: " + tree.height);
console.log("TREE size: " + tree.size);


tree = null;

// global.gc();

console.log(tree);
/*
tree.set("aa", 10);
tree.set("bb", 20);
tree.set("ab", 30);
tree.set("ba", 50);
tree.set("abc", 40);
*/
/*
for (const item of tree) {
  console.log(item);
}
*/
