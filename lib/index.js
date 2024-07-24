const RELEASE = "Release";
const DEBUG = "Debug";

function release(env) {
  switch (env) {
    case "development":
      return DEBUG;

    case "test":
    case "production":
    default:
      return RELEASE;
  }
}

const binding = require(`../build/${release(process.env.NODE_ENV)}/node_btree`);

module.exports = binding;
