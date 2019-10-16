import { BTree } from "../lib";

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

describe("Inheritance", () => {
  test("Create BTree instance", () => {
    const btree = new BTree(comparator);

    expect(btree).toBeInstanceOf(BTree);
  });
});

describe("Base functionality", () => {

  test("Check size property", () => {
    const btree = new BTree(comparator);

    btree.set("50", 50);
    btree.set("30", 30);
    btree.set("15", 150);

    expect(btree.size).toBe(3);
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
    expect(btree.size).toBe(2);
  });

  test('Items should be deletable if not exists', () => {
    const btree = new BTree(comparator);

    btree.set(50, 50);
    btree.set(30, 30);
    btree.set(15, 150);

    const result = btree.delete(31);

    expect(result).toBe(false);
    expect(btree.size).toBe(3);
  });

  test('Should be clearable by clear()', () => {
    const btree = new BTree<string, number>(comparator);

    btree.set("50", 51);
    btree.set("15", 150);
    btree.set("30", 30);

    expect(btree.size).toBe(3);

    btree.clear();

    expect(btree.size).toBe(0);
  });

  test('Should be processable after clear', () => {
    const btree = new BTree<string, number>(comparator);

    btree.set("50", 51);
    btree.set("15", 150);
    btree.set("30", 30);

    expect(btree.size).toBe(3);

    btree.clear();

    expect(btree.size).toBe(0);

    btree.set("One", 1);

    expect(btree.size).toBe(1);
  });

  test('Should be chackable by has method', () => {
    const btree = new BTree<string, number>(comparator);

    btree.set("50", 51);
    btree.set("15", 150);
    btree.set("30", 30);

    expect(btree.size).toBe(3);

    expect(btree.has("50")).toBe(true);
    expect(btree.has("100500")).toBe(false);
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

  test('Should be iterable by entries()', () => {
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
    const iterator = btree.entries();

    let expected;
    while (!(expected = checkIterator.next()).done) {
      const [key, value] = iterator.next().value;

      expect(key).toBe(expected.value.key);
      expect(value).toBe(expected.value.val);
    }
  });

  test('Should be iterable by forEach()', () => {
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

  test('Should be iterable by values()', () => {
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
    const iterator = btree.values();

    let expected;
    while (!(expected = checkIterator.next()).done) {
      const value = iterator.next().value;

      expect(value).toBe(expected.value.val);
    }
  });

  test('Should be iterable by keys()', () => {
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
    const iterator = btree.keys();

    let expected;
    while (!(expected = checkIterator.next()).done) {
      const value = iterator.next().value;

      expect(value).toBe(expected.value.key);
    }
  });

  test('Should be iterable if empty', () => {
    const btree = new BTree<string, number>(comparator);

    const result = btree.entries().next();

    expect(result.value).toBe(undefined);
    expect(result.done).toBe(true);
  });
});
