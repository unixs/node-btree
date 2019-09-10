import { BTree } from "../lib/binding/Debug/node-v72-linux-x64/node_btree";

function comparator(a: any, b: any): number {
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

describe("Base functionality", () => {
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

  test("Check get method (number => string)", () => {
    const btree = new BTree(comparator);

    btree.set("50", 50);
    btree.set("30", 30);
    btree.set("15", 150);


    expect(btree.get(15)).toBe(150);
  });


  test("Check get method (string => string)", () => {
    const btree = new BTree(comparator);

    btree.set("50", 50);
    btree.set("30", 30);
    btree.set("15", 150);


    expect(btree.get("15")).toBe(150);
  });

  test("Check get method (string => number)", () => {
    const btree = new BTree(comparator);

    btree.set(50, 50);
    btree.set(30, 30);
    btree.set(15, 150);


    expect(btree.get("15")).toBe(150);
  });

  test('Items should be deletable if exists', () => {
    const btree = new BTree(comparator);

    btree.set(50, 50);
    btree.set(30, 30);
    btree.set(15, 150);

    const result = btree.delete(30);

    expect(result).toBe(true);
    expect(btree.size()).toBe(2);
  });

  test('Items should be deletable if not exists', () => {
    const btree = new BTree(comparator);

    btree.set(50, 50);
    btree.set(30, 30);
    btree.set(15, 150);

    const result = btree.delete(31);

    expect(result).toBe(false);
    expect(btree.size()).toBe(3);
  });
});

describe('Traverse functionality', () => {
  test('Should be iterable by for-of', () => {
    const btree = new BTree<string, number>(comparator);

    btree.set("50", 51);
    btree.set("15", 150);
    btree.set("30", 30);

    const check = [
      {
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

    const checkIterator = check[Symbol.iterator]();

    for (const [key, val] of btree) {
      const check = checkIterator.next().value;

      expect(key).toBe(check.key);
      expect(val).toBe(check.val);
    }
  });

  test('Should be iterable by forEach', () => {
    const btree = new BTree<string, number>(comparator);

    btree.set("50", 51);
    btree.set("15", 150);
    btree.set("30", 30);

    const check = [
      {
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

    const checkIterator = check[Symbol.iterator]();

    btree.forEach((key, val) => {
      const check = checkIterator.next().value;

      expect(key).toBe(check.key);
      expect(val).toBe(check.val);
    });
  });
});
