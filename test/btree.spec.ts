import { BTree } from "../lib/binding/Debug/node-v72-linux-x64/node_btree.node";


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

let tree = new BTree(comparator);

const i = 50;

tree.set("50", 50);
tree.set("30", 30);
tree.set("15", 150);
//tree.set("20", 20);
//tree.set("15", 15);
//tree.set("30", 30);
//tree.set("10", 10);
//tree.set("3", 3);

//const one = tree.get("10");
const two = tree.get("150");
console.log(two);
//const three = tree.get("3");

console.log("TREE:" + tree);
console.log("TREE height: " + tree.height);
console.log("TREE size: " + tree.size());
