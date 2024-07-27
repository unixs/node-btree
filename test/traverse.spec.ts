/* eslint-disable @typescript-eslint/no-explicit-any */

import { BTree } from "..";
import { comparator, createBTree, initBtree, modernDescribe, MSG_TOO_FEW_ARGUMENTS } from "./common";

modernDescribe(describe);

describe("Traverse", () => {
  // @ts-expect-error 2339
  describe.modern("Iterator interface", () => {
    it("has next method", () => {
      const btree = createBTree().btree;
      const iterator = btree[Symbol.iterator]();

      expect(typeof iterator.next).toBe("function");
    });

    it("Iterator return value", () => {
      const { btree } = createBTree();
      const iterator = btree[Symbol.iterator]();

      expect(typeof iterator.next()).toBe("object");
    });

    it("Iterator result has correct fields", () => {
      const { btree, check } = createBTree();
      const iterator = btree[Symbol.iterator]();

      const result = iterator.next();

      expect(result.value[0]).toBe(check[0].key);
      expect(result.value[1]).toBe(check[0].val);
      expect(result.done).toBe(false);
    });
  });

  describe("Iteration methods", () => {
    describe("forEachReverse()", () => {
      it("is function", () => {
        const btree = new BTree(comparator);

        expect(typeof btree.forEachReverse).toBe("function");
      });

      it("iterable by forEachReverse()", () => {
        const btree = new BTree<string, number>(comparator);

        btree.set("50", 51);
        btree.set("15", 150);
        btree.set("30", 30);

        function* check(): Generator<
          {
            key: string;
            val: number;
            idx: number;
            revIdx: number;
          },
          {
            key: string;
            val: number;
            idx: number;
            revIdx: number;
          },
          unknown
        > {
          yield {
            key: "50",
            val: 51,
            idx: 0,
            revIdx: 2,
          };
          yield {
            key: "30",
            val: 30,
            idx: 1,
            revIdx: 1,
          };

          return {
            key: "15",
            val: 150,
            idx: 2,
            revIdx: 0,
          };
        }

        const checkIterator = check();

        btree.forEachReverse((val, key, idx, revIdx) => {
          const check = checkIterator.next().value;

          expect(key).toBe(check.key);
          expect(val).toBe(check.val);
          expect(idx).toBe(check.idx);
          expect(revIdx).toBe(check.revIdx);
        });
      });

      it("has value first arg", () => {
        const btree = initBtree();

        const check = [50, 30, 150];

        const checkIterator = check[Symbol.iterator]();

        btree.forEachReverse(val => {
          expect(val).toBe(checkIterator.next().value);
        });
      });

      it("has key second arg", () => {
        const btree = initBtree();

        const check = ["50", "30", "15"];

        const checkIterator = check[Symbol.iterator]();

        btree.forEachReverse((_val, key) => {
          expect(key).toBe(checkIterator.next().value);
        });
      });

      it("callback should have third idx arg", () => {
        const btree = initBtree();

        let i = 0;

        btree.forEachReverse((_val, _key, idx) => {
          expect(idx).toBe(i++);
        });
      });

      it("callback should have 4th rev_idx arg", () => {
        const btree = initBtree();

        let i = btree.size;

        btree.forEachReverse((_val, _key, _idx, rev_idx) => {
          expect(rev_idx).toBe(--i);
        });
      });

      it("recive context with basic function", () => {
        const btree = initBtree();

        btree.forEachReverse(
          function (this: { something: string }, val) {
            expect(this.something).toBe("test");

            return val;
          },
          { something: "test" },
        );
      });

      it("not recive context with arrow function", () => {
        const btree = initBtree();

        btree.forEachReverse(
          val => {
            expect(this).toMatchObject({});
            // @ts-expect-error 2532
            expect(this.test).toBe(undefined);

            return val;
          },
          { test: "test" },
        );
      });

      it("has Object context with arrow function", () => {
        const btree = initBtree();

        btree.forEachReverse(val => {
          expect(this).toMatchObject({});

          return val;
        });
      });

      it("has global context with basic function", () => {
        const btree = initBtree();

        btree.forEachReverse(function (this: any, val) {
          expect(this.process).toBeDefined();
          expect(this.process.nextTick).toBeDefined();
          expect(this.setTimeout).toBeDefined();

          return val;
        });
      });

      it("throw error if few arguments", () => {
        const btree = initBtree();

        // @ts-expect-error 2554
        expect(() => btree.forEachReverse()).toThrow(MSG_TOO_FEW_ARGUMENTS);
      });
    });

    describe("forEach()", () => {
      it("Should be iterable by forEach()", () => {
        const btree = new BTree<string, number>(comparator);

        btree.set("50", 51);
        btree.set("15", 150);
        btree.set("30", 30);

        const check = [
          {
            key: "15",
            val: 150,
          },
          {
            key: "30",
            val: 30,
          },
          {
            key: "50",
            val: 51,
          },
        ];

        const checkIterator = check[Symbol.iterator]();

        btree.forEach((val, key) => {
          const check = checkIterator.next().value;

          expect(key).toBe(check.key);
          expect(val).toBe(check.val);
        });
      });

      it("cb has value first arg", () => {
        const btree = initBtree();

        const check = [150, 30, 50];

        const checkIterator = check[Symbol.iterator]();

        btree.forEach(val => {
          expect(val).toBe(checkIterator.next().value);
        });
      });

      it("cb has key second arg", () => {
        const btree = initBtree();

        const check = ["15", "30", "50"];

        const checkIterator = check[Symbol.iterator]();

        btree.forEach((_val, key) => {
          expect(key).toBe(checkIterator.next().value);
        });
      });

      it("cb has third self arg", () => {
        const btree = initBtree();

        btree.forEach((_val, _key, self, _idx) => {
          expect(self).toBe(btree);
        });
      });

      it("cb has fourth idx arg", () => {
        const btree = initBtree();

        let i = 0;

        btree.forEach((_val, _key, _self, idx) => {
          expect(idx).toBe(i++);
        });
      });

      it("recive context with basic function", () => {
        const btree = initBtree();

        btree.forEach(
          function (this: any, val) {
            expect(this.something).toBe("test");

            return val;
          },
          { something: "test" },
        );
      });

      it("not recive context with arrow function", () => {
        const btree = initBtree();

        btree.forEach(
          val => {
            expect(this).toMatchObject({});
            // @ts-expect-error 2532
            expect(this.test).toBe(undefined);

            return val;
          },
          { test: "test" },
        );
      });

      it("has Object context with arrow function", () => {
        const btree = initBtree();

        btree.forEach(val => {
          expect(this).toMatchObject({});

          return val;
        });
      });

      it("has global context with basic function", () => {
        const btree = initBtree();

        btree.forEach(function (this: any, val) {
          expect(this.process).toBeDefined();
          expect(this.process.nextTick).toBeDefined();
          expect(this.setTimeout).toBeDefined();

          return val;
        });
      });

      it("throw error if few arguments", () => {
        const btree = initBtree();

        // @ts-expect-error 2554
        expect(() => btree.forEach()).toThrow(MSG_TOO_FEW_ARGUMENTS);
      });
    });

    // @ts-expect-error 2339
    describe.modern("Itrators", () => {
      it("Should be iterable by for-of", () => {
        const btree = new BTree(comparator);

        btree.set("50", 51);
        btree.set("15", 150);
        btree.set("30", 30);

        const check = [
          {
            key: "15",
            val: 150,
          },
          {
            key: "30",
            val: 30,
          },
          {
            key: "50",
            val: 51,
          },
        ];

        const checkIterator = check[Symbol.iterator]();

        for (const [key, val] of btree) {
          const check = checkIterator.next().value;

          expect(key).toBe(check.key);
          expect(val).toBe(check.val);
        }
      });

      it("Should be iterable by entries()", () => {
        const btree = new BTree(comparator);

        btree.set("50", 51);
        btree.set("15", 150);
        btree.set("30", 30);

        const check = [
          {
            key: "15",
            val: 150,
          },
          {
            key: "30",
            val: 30,
          },
          {
            key: "50",
            val: 51,
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

      it("Should be iterable by values()", () => {
        const btree = new BTree(comparator);

        btree.set("50", 51);
        btree.set("15", 150);
        btree.set("30", 30);

        const check = [
          {
            key: "15",
            val: 150,
          },
          {
            key: "30",
            val: 30,
          },
          {
            key: "50",
            val: 51,
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

      it("Should be iterable by keys()", () => {
        const btree = new BTree(comparator);

        btree.set("50", 51);
        btree.set("15", 150);
        btree.set("30", 30);

        const check = [
          {
            key: "15",
            val: 150,
          },
          {
            key: "30",
            val: 30,
          },
          {
            key: "50",
            val: 51,
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

      it("Should be iterable if empty", () => {
        const btree = new BTree(comparator);

        const result = btree.entries().next();

        expect(result.value).toBe(undefined);
        expect(result.done).toBe(true);
      });
    });
  });
});
