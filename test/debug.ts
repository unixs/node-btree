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

const sz: number = btree.size;
const ht: number = btree.height;
const res1: number|undefined = btree.get(40);
const res: boolean = btree.delete(40);
const res2: boolean = btree.has(40);
const btreeref: BTree<number, number> = btree.set(40, 40);

const arr1: string[] = btree.map((k, v) => "" + k +"-"+ v);
console.log(arr1);

const arr: string = btree.reduce<string>((prev, v, k, idx) => prev+idx+k+v, "");
console.log(arr);

const arr2 = btree.filter((v, k, idx) => k > 50);
console.log(arr2.toMap(), btree.toMap());

btree.forEach((...argv) => {
  console.log(argv);
});

btree.forEachReverse((...argv) => {
  console.log(argv);
});

btree.toArrays().forEach(([k, v]) => {
  console.log(k, v);
});

btree.toArray().forEach((obj) => {
  console.log(obj);
});

btree.flatten().forEach((item) => {
  console.log(item);
});

const arr3 = btree.getKeys();
const arr4 = btree.getValues();

console.log(arr3, arr4);

const ft = btree.first();
const lt = btree.last();

console.log(ft, lt);

const bfo = btree.before(60);
const aft = btree.after(60);
console.log(bfo.toMap(), aft.toMap());

const btw = btree.between(30, 60);
console.log(btw.toMap());

for (const [key, value] of btree) {
  console.log(key, value);
}

for (const [k, v] of btree.entries()) {
  console.log(k, v);
}

for (const value of btree.values()) {
  console.log(value);
}

for (const key of btree.keys()) {
  console.log(key);
}

const it = btree.entries();

const v1 = it.next().value;

const m5: Map<string, number> = new Map();
const m5i = m5.entries();
const v2 = it.next().value;
