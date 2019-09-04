import { BTree } from "../lib/binding/Debug/node-v72-linux-x64/node_btree.node";
import { toUnicode } from "punycode";

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

describe("Sample", () => {
  test("Create BTree instance", () => {
    const btree = new BTree(comparator);

    expect(btree).toBeInstanceOf(BTree);
  });

  test.todo('Check');

  test("Check size method", () => {
    const btree = new BTree(comparator);

    btree.set("50", 50);
    btree.set("30", 30);
    btree.set("15", 150);

    expect(btree.size()).toBe(3);
  });

  test("Check heigth field", () => {
    const btree = new BTree(comparator);

    btree.set("50", 50);
    btree.set("30", 30);
    btree.set("15", 150);

    expect(btree.height).toBe(2);
  });

  test("Check get method", () => {
    const btree = new BTree(comparator);

    btree.set("50", 50);
    btree.set("30", 30);
    btree.set("15", 150);


    expect(btree.get(15)).toBe(150);
  });
});

