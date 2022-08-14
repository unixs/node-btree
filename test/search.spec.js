const { BTree } = require("../lib");

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

  btree.set(15, 150);
  btree.set(50, 500);
  btree.set(30, 300);
  btree.set(40, 400);
  btree.set(45, 450);
  btree.set(11, 110);

  return btree;
}

describe("Search methods", () => {
  let btree;

  beforeAll(() => {
    btree = initBtree();
  });

  describe("before", () => {
    it("is function", () => {
      expect(typeof btree.before).toBe("function");
    });

    it("return BTree", () => {
      expect(btree.before(30) instanceof BTree).toBe(true);
    });

    it("return correct result", () => {
      function* checkgGenerator() {
        yield { key: 11, value: 110 };
        yield { key: 15, value: 150 };
        yield { key: 30, value: 300 };
        yield { key: 40, value: 400 };
      }

      const it = checkgGenerator();

      const result = btree.before(40);

      expect(result.size).toBe(4);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it("return correct result with skip", () => {
      function* checkgGenerator() {
        yield { key: 11, value: 110 };
        yield { key: 15, value: 150 };
        yield { key: 30, value: 300 };
      }

      const it = checkgGenerator();

      const result = btree.before(40, true);

      expect(result.size).toBe(3);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it('return undefined if key not found', () => {
      expect(() => btree.before(33)).toThrow("key not found");
    });

    it('return 1 on edge', () => {
      expect(btree.before(11).size).toBe(1);
    });

    it('return empty on edge with skip', () => {
      expect(btree.before(11, true).size).toBe(0);
    });
  });

  describe("between", () => {
    it("is function", () => {
      expect(typeof btree.between).toBe("function");
    });

    it("return BTree", () => {
      expect(btree.between(15, 45) instanceof BTree).toBe(true);
    });

    it("return correct result", () => {
      function* checkgGenerator() {
        yield { key: 15, value: 150 };
        yield { key: 30, value: 300 };
        yield { key: 40, value: 400 };
        yield { key: 45, value: 450 };
      }

      const it = checkgGenerator();

      const result = btree.between(15, 45);

      expect(result.size).toBe(4);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it("return correct result with skip", () => {
      function* checkgGenerator() {
        yield { key: 30, value: 300 };
        yield { key: 40, value: 400 };
      }

      const it = checkgGenerator();

      const result = btree.between(15, 45, true);

      expect(result.size).toBe(2);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it('throw error if first key not found', () => {
      expect(() => btree.between(12, 30)).toThrow("lower key not found in tree");
    });

    it('throw error if second key not found', () => {
      expect(() => btree.between(11, 31)).toThrow("upper key not found in tree");
    });

    it('throw error key1 == key2', () => {
      function* checkgGenerator() {
        yield { key: 30, value: 300 };
      }

      const it = checkgGenerator();

      const result = btree.between(30, 30);

      expect(result.size).toBe(1);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it('throw error key1 == key2 with skip', () => {
      const result = btree.between(30, 30, true);

      expect(result.size).toBe(0);
    });

    it('throw error key1 > key2', () => {
      function* checkgGenerator() {
        yield { key: 15, value: 150 };
        yield { key: 30, value: 300 };
        yield { key: 40, value: 400 };
        yield { key: 45, value: 450 };
        yield { key: 50, value: 500 };
      }

      const it = checkgGenerator();

      const result = btree.between(50, 15);

      expect(result.size).toBe(5);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it('throw error key1 > key2 with skip', () => {
      function* checkgGenerator() {
        yield { key: 30, value: 300 };
        yield { key: 40, value: 400 };
        yield { key: 45, value: 450 };
      }

      const it = checkgGenerator();

      const result = btree.between(50, 15, true);

      expect(result.size).toBe(3);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it("return empty result without middle elements", () => {
      function* checkgGenerator() {
        yield { key: 30, value: 300 };
        yield { key: 40, value: 400 };
      }

      const it = checkgGenerator();

      const result = btree.between(30, 40);

      expect(result.size).toBe(2);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it("return empty result without middle elements with skip", () => {
      const result = btree.between(30, 40, true);

      expect(result.size).toBe(0);
    });
  });

  describe("after", () => {
    let btree;

    beforeEach(() => {
      btree = initBtree();
    });

    it("is function", () => {
      expect(typeof btree.after).toBe("function");
    });


    it("return BTree", () => {
      expect(btree.after(30) instanceof BTree).toBe(true);
    });

    it("return correct result", () => {
      function* checkgGenerator() {
        yield { key: 30, value: 300 };
        yield { key: 40, value: 400 };
        yield { key: 45, value: 450 };
        yield { key: 50, value: 500 };
      }

      const it = checkgGenerator();

      const result = btree.after(30);

      expect(result.size).toBe(4);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it("return correct result with skip", () => {
      function* checkgGenerator() {
        yield { key: 40, value: 400 };
        yield { key: 45, value: 450 };
        yield { key: 50, value: 500 };
      }

      const it = checkgGenerator();

      const result = btree.after(30, true);

      expect(result.size).toBe(3);

      for (const [k, v] of result) {
        const next = it.next().value;

        expect(k).toBe(next.key);
        expect(v).toBe(next.value);
      }
    });

    it('return undefined if key not found', () => {
      expect(() => btree.after(33)).toThrow("key not found");
    });

    it('return 1 on edge', () => {
      expect(btree.after(50).size).toBe(1);
    });

    it('return empty on edge with skip', () => {
      expect(btree.after(50, true).size).toBe(0);
    });
  });
});
