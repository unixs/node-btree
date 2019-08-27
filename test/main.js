const binary = require('node-pre-gyp');
const path = require('path');
const binding_path = binary.find(path.resolve(path.join(__dirname, '../package.json')));
const ext = require("../lib/binding/Debug/node-v72-linux-x64/node_mixin.node");

console.log("node-mixin test.");

console.log(ext.hello());
require('assert')
  .equal(ext.hello(), "hello");
