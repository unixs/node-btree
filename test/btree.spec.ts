/* eslint-disable @typescript-eslint/no-explicit-any */

import { BTree } from "..";

import { comparator, initBtree, modernDescribe, MSG_TOO_FEW_ARGUMENTS } from "./common";

modernDescribe(describe);

describe("Introspection", () => {
  it("Respond to Symbol.toStringTag", () => {
    const btree = initBtree();

    expect(typeof btree[Symbol.toStringTag]).toBe("string");
  });

  it("Construct correct default name", () => {
    const btree = initBtree();

    expect(Object.prototype.toString.call(btree)).toBe("[object BTree]");
  });
});

describe("Static methods", () => {
  describe("BTree.from()", () => {
    it("from() method should be", () => {
      expect(BTree.from.constructor.name).toBe("Function");
    });

    it("from() method should return BTree instance", () => {
      const btree = BTree.from(comparator, []);

      expect(btree).toBeInstanceOf(BTree);
      expect(btree.size).toBe(0);
    });

    it("from() should throw error without args", () => {
      // @ts-expect-error 2554
      expect(() => BTree.from()).toThrow(MSG_TOO_FEW_ARGUMENTS);
    });

    it("from() should throw error if comparator is bad", () => {
      // @ts-expect-error 2345
      expect(() => BTree.from([], [])).toThrow("First arg must be comparator qsort() like function");
    });

    it("from() should throw error if second arg is bad", () => {
      expect(() => BTree.from(comparator, "")).toThrow("Second arg must be Array, Map or iterable");
    });

    it("from() key-value-able objects array", () => {
      const arr = [
        {
          key: 10,
          value: "100",
          a: "a",
        },
        {
          key: 20,
          value: "200",
          b: "b",
        },
        {
          key: 30,
          value: "300",
          c: "c",
        },
      ];

      const btree = BTree.from(comparator, arr);

      expect(btree.size).toBe(3);
      expect(btree.height).toBe(2);
      expect(btree.get(20)).toBe("200");
    });

    it("from() array of arrays [key, value]", () => {
      const arr = [
        [10, "10"],
        [30, "30"],
        [80, "80"],
        [20, "20"],
        [50, "50"],
      ];

      const btree = BTree.from(comparator, arr);

      expect(btree.size).toBe(5);
      expect(btree.height).toBe(3);
      expect(btree.get(30)).toBe("30");
    });

    it("from() mixed array of arrays & objects", () => {
      const arr = [
        [10, "10"],
        {
          key: 30,
          value: "30",
        },
        [80, "80"],
        {
          key: 20,
          value: "20",
        },
        [50, "50"],
      ];

      const btree = BTree.from(comparator, arr);

      expect(btree.size).toBe(5);
      expect(btree.height).toBe(3);
      expect(btree.get(30)).toBe("30");
      expect(btree.get(50)).toBe("50");
    });

    it("from() generator", () => {
      function* generator() {
        for (let i = 0; i < 1000; i++) {
          yield {
            key: i,
            value: `${i} bla-bla-bla`,
          };
        }
      }

      const btree = BTree.from(comparator, generator());

      expect(btree.size).toBe(1000);
      expect(btree.height).toBe(10);
      expect(btree.get(30)).toBe("30 bla-bla-bla");
      expect(btree.get(50)).toBe("50 bla-bla-bla");
    });

    // Temporary skipped
    // BUG: https://github.com/facebook/jest/issues/2549
    it.skip("from() Map instance", () => {
      const map = new Map();

      map.set(10, "10");
      map.set(30, "30");
      map.set(80, "80");
      map.set(20, "20");
      map.set(50, "50");

      const btree = BTree.from(comparator, map);

      expect(btree.size).toBe(5);
      expect(btree.height).toBe(3);
      expect(btree.get(30)).toBe("30");
    });
  });
});

describe("Base functionality", () => {
  describe("BTree specific", () => {
    it("Should have correct height field without nodes", () => {
      const btree = new BTree(comparator);

      expect(btree.height).toBe(0);
    });

    it("Should have correct heigth field with nodes", () => {
      const btree = initBtree();

      expect(btree.height).toBe(2);
    });
  });

  describe("Map() interface", () => {
    it("Check size property if empty", () => {
      const btree = new BTree(comparator);

      expect(btree.size).toBe(0);
    });

    it("Check size property if has values", () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);
    });

    it("get method should be callable without args", () => {
      const btree = new BTree(comparator);

      expect(btree.get()).toBe(undefined);
    });

    it("Check get method (number => string)", () => {
      const btree = initBtree();

      expect(btree.get("15")).toBe(150);
    });

    it("Check get method (string => string)", () => {
      const btree = initBtree();

      expect(btree.get("15")).toBe(150);
    });

    it("Check get method (string => number)", () => {
      const btree = new BTree(comparator);

      btree.set("50", 50);
      btree.set("30", 30);
      btree.set("15", 150);

      expect(btree.get("15")).toBe(150);
    });
    it("Should be addable if key exists", () => {
      const btree = initBtree();

      btree.set("30", 40);

      expect(btree.get("30")).toBe(40);
    });

    it("Should accept set() without args", () => {
      const btree = new BTree(comparator);

      btree.set();

      expect(btree.size).toBe(1);
      expect(btree.get(undefined) === undefined).toBeTruthy();
      expect(btree.get() === undefined).toBeTruthy();
    });

    it("Should accept set() with one first arg", () => {
      const btree = new BTree(comparator);

      btree.set("1");

      expect(btree.size).toBe(1);
      expect(btree.get("1") === undefined).toBeTruthy();
    });

    it("Values should be replaceable by set()", () => {
      const btree = new BTree(comparator);

      btree.set("1", 2);
      btree.set("3", 4);
      btree.set("5", 6);

      expect(btree.size).toBe(3);
      expect(btree.get("3")).toBe(4);

      btree.set("3", 7);

      expect(btree.size).toBe(3);
      expect(btree.get("3")).toBe(7);
    });

    it("deletable if arg is missing", () => {
      const btree = new BTree((a: string, b: string) => {
        if (a === undefined) {
          return -1;
        }

        if (a > b) {
          return 1;
        }
        else if (a < b) {
          return -1;
        }
        else {
          return 0;
        }
      });

      btree.set("50", 50);
      btree.set("30", 30);
      btree.set("15", 150);

      // @ts-expect-error 2554
      const result = btree.delete(); // arg is undefined

      expect(btree.size).toBe(3);
      expect(result).toBe(false);
    });

    it("Items should be deletable if exists", () => {
      const btree = initBtree();

      const result = btree.delete("30");

      expect(result).toBe(true);
      expect(btree.size).toBe(2);
    });

    it("Items should be deletable if not exists", () => {
      const btree = initBtree();

      const result = btree.delete("31");

      expect(result).toBe(false);
      expect(btree.size).toBe(3);
    });

    it("Should be clearable by clear()", () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);

      btree.clear();

      expect(btree.size).toBe(0);
    });

    it("Should be processable after clear", () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);

      btree.clear();

      expect(btree.size).toBe(0);

      btree.set("One", 1);

      expect(btree.size).toBe(1);
    });

    it("has() method should be callable", () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);

      expect(btree.has("50")).toBe(true);
      expect(btree.has("100500")).toBe(false);
    });

    it("has() method should be callable without args", () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);

      // @ts-expect-error 2554
      expect(btree.has()).toBe(true);
    });

    it("has() method should be callable without args and find undefined", () => {
      const btree = initBtree();

      btree.set();

      // @ts-expect-error 2554
      expect(btree.has()).toBe(true);
    });
  });
});
