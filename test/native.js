const { BTree } = require("../lib/binding/Debug/node-v72-linux-x64/node_btree.node");

/*
const arr1 = [];
let arr2 = [];

for (let i = 1; i <= 1000000; i++) {
  arr1.push({ i });
}

arr1.forEach((v) => arr2.push(v));

arr2 = null;

console.log("end");
*/
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

let btree = new BTree(comparator);

//for (let i = 1; i <= 3000000; i++) {
//  btree.set(i, i);
//}
// console.log(btree.height);

btree.set("50", 51);
btree.set("15", 150);
btree.set("30", 30);
btree.set("18", 81);
btree.set("100", 0);

btree.delete("100");

btree = null;

global.gc();

const m = new Map();

m.set(1, 2);


console.log(m);
/*
btree.forEach((k, v) => {
  console.log(k, v);
});

for (const val of btree) {
  console.log(val);
}

*/
