function release() {
  const RELEASE = 'Release';
  const DEBUG = 'Debug';

  switch (process.env.NODE_ENV) {
    case 'development':
      return DEBUG;

    case 'test':
    case 'production':
    default:
      return RELEASE;
  }
}

const binding = require(`../build/${release()}/node_btree`);

module.exports = binding;
