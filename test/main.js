const binary = require('node-pre-gyp');
const path = require('path');
const binding_path = binary.find(path.resolve(path.join(__dirname, '../package.json')));
const ext = require("../lib/binding/Debug/node-v72-linux-x64/node_mixin.node");

console.log("node-mixin test.");

console.log(ext.hello());
require('assert')
  .equal(ext.hello(), "hello");


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

