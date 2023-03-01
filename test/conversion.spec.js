const { BTree, GLIB_VERSION: { hasGTreeNode } } = require("../lib");


const modernGlibOnly = (callback) => {
  if (hasGTreeNode()) {
    callback.call(this);
  }
};


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

function initBtree() {
  const btree = new BTree(comparator);

  btree.set(10, 10);
  btree.set(20, 20);
  btree.set(50, 50);
  btree.set(30, 30);
  btree.set(150, 150);

  return btree;
}

function* checkgGenerator() {
  yield { key: 11, value: 110 };
  yield { key: 15, value: 150 };
  yield { key: 30, value: 300 };
  yield { key: 40, value: 400 };
}

describe("Conversion methods", () => {
  let btree;

  beforeEach(() => {
    btree = initBtree();
  });

  it.todo("toArray()");
  it.todo("flatten()");

  modernGlibOnly(() => {
    describe("toMap()", () => {
      it("toMap() should be callable", () => {
        const btree = initBtree();

        expect(typeof btree.toMap).toBe("function");
      });

      it.skip("toMap() should return Map instance", () => {
        const btree = initBtree();
        const map = btree.toMap();

        expect(map).toBeInstanceOf(Map);
      });

      it("toMap() should return expected map k=>v pairs", () => {
        const btree = initBtree();
        const map = btree.toMap();

        expect(map.get("50")).toBe(50);
        expect(map.get("15")).toBe(150);
        expect(map.get("30")).toBe(30);
      });
    });

    describe("toSet()", () => {
      it("toSet() should be callable", () => {
        const btree = initBtree();

        expect(btree.toSet.constructor.name).toBe("Function");
      });

      it.skip("toSet() should return Set instance", () => {
        const btree = initBtree();
        const set = btree.toSet();

        expect(set).toBeInstanceOf(Set);
      });

      it("toSet() should return expected values", () => {
        const btree = initBtree();
        const set = btree.toSet();

        expect(set.has(50)).toBe(true);
        expect(set.has(150)).toBe(true);
        expect(set.has(30)).toBe(true);
        expect(set.has(42)).toBe(false);
      });
    });
  });

  describe("toArrays()", () => {
    it('method callable', () => {
      expect(typeof btree.toArrays).toBe("function");
    });

    // eslint-disable-next-line jest/expect-expect
    it('return expected result', () => {
      const arr = [
        [10, 10],
        [20, 20],
        [30, 30],
        [50, 50],
        [150, 150]
      ];

      const result = btree.toArrays();

      compare(result, arr[Symbol.iterator](), arr.length);
    });

    it('return expected result for empty tree', () => {
      btree = new BTree(comparator);
    });
  });
});
