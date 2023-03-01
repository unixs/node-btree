const { BTree, GLIB_VERSION } = require("../lib");

function initBtree() {
  const btree = new BTree((a, b) => a - b);

  btree.set(50, 500);
  btree.set(30, 300);
  btree.set(15, 150);
  btree.set(60, 600);
  btree.set(40, 400);
  btree.set(150, 1500);

  return btree;
}

const btree = initBtree();

console.log(btree.toMap());
console.log(btree.between(60, 15, true).getKeys());
console.log(btree.getValues());
