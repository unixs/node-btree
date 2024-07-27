import { BTree } from "..";

import { modernDescribe } from "./common";

function comparator(a: number, b: number) {
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

  btree.set(10, 100);
  btree.set(20, 200);
  btree.set(50, 500);
  btree.set(30, 300);
  btree.set(150, 1500);

  return btree;
}

modernDescribe(describe);

describe("Conversion methods", () => {
  let btree: BTree<number, number>;

  beforeEach(() => {
    btree = initBtree();
  });

  it.todo("Dummy test");

  // @ts-expect-error 2339
  describe.modern("Modern methods", () => {
    describe("toMap()", () => {
      it("toMap() should be callable", () => {
        const btree = initBtree();

        expect(typeof btree.toMap).toBe("function");
      });

      it("toMap() should return Map instance", () => {
        const btree = initBtree();
        const map = btree.toMap();

        expect(map.constructor.name).toBe("Map");
      });

      it("toMap() should return expected map k=>v pairs", () => {
        const btree = initBtree();
        const map = btree.toMap();

        expect(map.get(50)).toBe(500);
        expect(map.get(150)).toBe(1500);
        expect(map.get(30)).toBe(300);
      });
    });

    describe("toSet()", () => {
      it("toSet() should be callable", () => {
        const btree = initBtree();

        expect(btree.toSet.constructor.name).toBe("Function");
      });

      it("toSet() should return Set instance", () => {
        const btree = initBtree();
        const set = btree.toSet();

        expect(set.constructor.name).toBe("Set");
      });

      it("toSet() should return expected values", () => {
        const btree = initBtree();
        const set = btree.toSet();

        expect(set.has(500)).toBe(true);
        expect(set.has(1500)).toBe(true);
        expect(set.has(300)).toBe(true);
        expect(set.has(42)).toBe(false);
      });
    });

    describe("toArrays()", () => {
      it("method callable", () => {
        expect(typeof btree.toArrays).toBe("function");
      });

      it("result is array", () => {
        const result = btree.toArrays();

        expect(result.constructor.name).toBe("Array");
      });

      it("result is array of arrays", () => {
        const result = btree.toArrays();

        expect(result[0].constructor.name).toBe("Array");
      });

      it("return expected result", () => {
        const arr = [
          [10, 100],
          [20, 200],
          [30, 300],
          [50, 500],
          [150, 1500],
        ];

        const result = btree.toArrays();
        const it = arr[Symbol.iterator]();

        expect(result.length).toBe(5);

        for (const [k, v] of result) {
          const [key, val] = it.next().value;

          expect(k).toBe(key);
          expect(v).toBe(val);
        }
      });

      it("return expected result for empty tree", () => {
        btree = new BTree(comparator);

        const result = btree.toArrays();

        expect(result.length).toBe(0);
      });
    });

    describe("toArray()", () => {
      it("method callable", () => {
        expect(typeof btree.toArray).toBe("function");
      });

      it("result is array", () => {
        const result = btree.toArray();

        expect(result.constructor.name).toBe("Array");
      });

      it("result is array of objects", () => {
        const result = btree.toArray();

        expect(result[0].constructor.name).toBe("Object");
      });

      it("return expected result", () => {
        const arr = [
          { key: 10, value: 100 },
          { key: 20, value: 200 },
          { key: 30, value: 300 },
          { key: 50, value: 500 },
          { key: 150, value: 1500 },
        ];

        const result = btree.toArray();
        const it = arr[Symbol.iterator]();

        expect(result.length).toBe(5);

        for (const { key, value } of result) {
          const { key: k, value: v } = it.next().value;

          expect(key).toBe(k);
          expect(value).toBe(v);
        }
      });

      it("return expected result for empty tree", () => {
        btree = new BTree(comparator);

        const result = btree.toArray();

        expect(result.length).toBe(0);
      });
    });

    describe("flatten()", () => {
      it("method callable", () => {
        expect(typeof btree.flatten).toBe("function");
      });

      it("result is array", () => {
        const result = btree.flatten();

        expect(result.constructor.name).toBe("Array");
      });

      it("result is array of numbers", () => {
        const result = btree.flatten();

        expect(typeof result[0]).toBe("number");
      });

      it("return expected result", () => {
        const arr = [10, 100, 20, 200, 30, 300, 50, 500, 150, 1500];

        const result = btree.flatten();
        const it = arr[Symbol.iterator]();

        expect(result.length).toBe(10);

        for (const item of result) {
          const i = it.next().value;

          expect(item).toBe(i);
        }
      });

      it("return expected result for empty tree", () => {
        btree = new BTree(comparator);

        const result = btree.toArray();

        expect(result.length).toBe(0);
      });
    });
  });
});
