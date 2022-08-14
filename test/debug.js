const { BTree, GLIB_VERSION } = require("../lib");

function initBtree() {
  const btree = new BTree((a, b) => a - b);

  btree.set(50, 50);
  btree.set(30, 30);
  btree.set(15, 15);
  btree.set(60, 60);
  btree.set(40, 40);
  btree.set(150, 150);

  return btree;
}

const btree = initBtree();

console.log(btree.toMap());
console.log(btree.between(60, 15, true).toMap());
