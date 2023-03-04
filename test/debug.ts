import { BTree } from "..";

function comparator(a: number, b: number): number {
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
  const btree = new BTree<number, number>(comparator);

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

btree.delete(30);

console.log(btree.toMap(), btree.size);

const it = btree[Symbol.iterator]();

const val = it.next().value;

console.log(val.value);
