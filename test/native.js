const { BTree } = require("../lib/binding/Debug/node-v72-linux-x64/node_btree.node");

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

const check = [{
    key: "15",
    val: 150
  },
  {
    key: "30",
    val: 30
  },
  {
    key: "50",
    val: 51
  },
];

const m = new Map();

const checkIterator = check[Symbol.iterator]();
for (const [key, val] of btree) {
  const check = checkIterator.next().value;

  expect(key).toBe(check.key);
  expect(val).toBe(check.val);
}
