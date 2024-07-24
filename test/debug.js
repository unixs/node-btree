/* eslint-disable @typescript-eslint/no-var-requires */

const { BTree } = require("..");

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

function initBtree() {
  const btree = new BTree(comparator);

  btree.set(50, 500);
  btree.set(30, 300);
  btree.set(15, 150);
  btree.set(60, 600);
  btree.set(40, 400);
  btree.set(150, 1500);

  return btree;
}

const btree = initBtree();

console.log(btree.toMap(), btree.size);

btree.forEach(console.log);

btree.clear();

btree.forEach(console.log);

console.log(btree.toMap(), btree.size);
