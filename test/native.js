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

const btree = new BTree(comparator);

btree.set("50", 51);
btree.set("15", 150);
btree.set("30", 30);
btree.set("18", 81);
btree.set("100", 0);

let i = 1;
btree.forEach((k, v) => {
  if (i == 1) {
    btree.delete("30");
    btree.delete("18");
    btree.delete("15");
    btree.delete("50");
    btree.delete("100");
  }
  else if (i == 5) {
    //btree.set("800", 800);
    //btree.set("25", 25);
  }

  console.log(k, v);
  i++;
});
