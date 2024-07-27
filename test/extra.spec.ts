/* eslint-disable @typescript-eslint/no-explicit-any */

import { BTree } from "..";
import { comparator, initBtree, initGenerator, modernDescribe, MSG_TOO_FEW_ARGUMENTS } from "./common";

modernDescribe(describe);

describe("Extra methods", () => {
  describe("filter()", () => {
    it("filter() is callable", () => {
      const btree = initBtree();

      expect(btree.filter.constructor.name).toBe("Function");
    });

    it("filter() returns BTree", () => {
      const btree = initBtree();

      expect(btree.filter(() => true).constructor.name).toBe("BTree");
    });

    it("filter() returns all items", () => {
      const btree = initBtree();

      expect(btree.filter(() => true).size).toBe(3);
    });

    it("filter() returns 0 items", () => {
      const btree = initBtree();

      expect(btree.filter(() => false).size).toBe(0);
    });

    it("filter() callback has first value arg", () => {
      const btree = initBtree();

      const it = initGenerator();

      btree.filter(val => {
        expect(val).toBe(it.next().value.value);

        return true;
      });
    });

    it("filter() callback has second key arg", () => {
      const btree = initBtree();

      const it = initGenerator();

      btree.filter((_val, key) => {
        expect(key).toBe(it.next().value.key);

        return true;
      });
    });

    it("filter() callback has third idx arg", () => {
      const btree = initBtree();

      let i = 0;

      btree.filter((_val, _key, idx) => {
        expect(idx).toBe(i++);

        return true;
      });
    });

    it("filter() recives the context with basic function", () => {
      const btree = initBtree();

      btree.filter(
        function (this: any, val) {
          expect(this.test).toBe("test");

          return Boolean(val);
        },
        { test: "test" },
      );
    });
  });

  describe("reduce()", () => {
    it("reduce() is callable & returns valid value for numbers", () => {
      const btree = initBtree();

      const result = btree.reduce<number>((acc, val, _key, _idx, _btree) => {
        return acc + val;
      }, 3);

      expect(result).toBe(233);
    });

    it("reduce() callback should have first value arg", () => {
      const btree = initBtree();

      const array = btree.reduce<number[]>((acc, val) => {
        acc.push(val);

        return acc;
      }, []);

      expect(array.length).toBe(3);
      expect(array[2]).toBe(50);
    });

    it("reduce() callback should have second key arg", () => {
      const btree = initBtree();

      const str = btree.reduce<string>((acc, _val, key) => {
        return acc + key;
      }, "");

      expect(str.length).toBe(6);
      expect(str).toBe("153050");
    });

    it("reduce() callback should have third idx arg", () => {
      const btree = initBtree();

      const array = btree.reduce<number[]>((acc, _val, _key, idx) => {
        acc.push(idx);

        return acc;
      }, []);

      expect(array.length).toBe(3);

      for (let i = 0; i < 3; i++) {
        expect(array[i]).toBe(i);
      }
    });

    it("reduce() callback should have fourth bTree arg", () => {
      const btree = initBtree();

      const result = btree.reduce<BTree<string, number>>((_acc, _val, _key, _idx, btree) => btree, null);

      expect(result).toBeInstanceOf(BTree);
      expect(result.size).toBe(3);
      expect(result.get("15")).toBe(150);
    });

    it("reduce() should throw error if few arguments", () => {
      const btree = initBtree();

      expect(() => btree.reduce((_acc, val) => val)).toThrow(MSG_TOO_FEW_ARGUMENTS);
    });
  });

  describe("map()", () => {
    it("map() is callable & returns an array", () => {
      const btree = initBtree();

      const array = btree.map(val => val);

      expect(array.constructor.name).toBe("Array");
      expect(array.length).toBe(3);
    });

    it("map() callback has first value arg", () => {
      const btree = initBtree();

      const array = btree.map(val => val);

      expect(array[0]).toBe(150);
      expect(array.length).toBe(3);
    });

    it("map() callback has second key arg", () => {
      const btree = initBtree();

      const array = btree.map((_val, key) => key);

      expect(array[1]).toBe("30");
      expect(array.length).toBe(3);
    });

    it("map() callback has third idx arg", () => {
      const btree = initBtree();

      const array = btree.map((_val, _key, idx) => idx);

      expect(array.length).toBe(3);

      for (let i = 0; i < 3; i++) {
        expect(array[i]).toBe(i);
      }
    });

    it("map() callback has fourth bTree arg", () => {
      const btree = initBtree();

      const array = btree.map((_val, _key, _idx, btree) => btree);

      expect(array[0]).toBeInstanceOf(BTree);
      expect(array.length).toBe(3);
      expect(array[1].get("15")).toBe(150);
    });

    it("map() recives the context with basic function", () => {
      const btree = initBtree();

      btree.map(
        function (this: any, val) {
          expect(this.test).toBe("test");

          return val;
        },
        { test: "test" },
      );
    });

    it("map() does not recive context with arrow function", () => {
      const btree = initBtree();

      btree.map(
        val => {
          expect(this).toMatchObject({});
          // @ts-expect-error 2532
          expect(this.test).toBe(undefined);

          return val;
        },
        { test: "test" },
      );
    });

    it("map() has Object context with arrow function", () => {
      const btree = initBtree();

      btree.map(val => {
        expect(this).toMatchObject({});

        return val;
      });
    });

    it("map() has global context with basic function", () => {
      const btree = initBtree();

      btree.map(function (this: any, val) {
        expect(this.process).toBeDefined();
        expect(this.process.nextTick).toBeDefined();
        expect(this.setTimeout).toBeDefined();

        return val;
      });
    });

    it("map() throw error if few arguments", () => {
      const btree = initBtree();

      // @ts-expect-error 2554
      expect(() => btree.map()).toThrow(MSG_TOO_FEW_ARGUMENTS);
    });

    it("has the ability to remove node in the callback", () => {
      const btree = initBtree();

      const result = btree.map(v => {
        btree.delete("30");

        return v;
      });

      expect(result.length).toBe(3);
      expect(result[1]).toBe(50);
      expect(result[2]).toBeUndefined();
    });

    // @ts-expect-error 2339
    describe.modern("Extraction methods", () => {
      let btree: BTree<string, number>;

      beforeEach(() => {
        btree = initBtree();
      });

      describe("getKeys()", () => {
        it("method callable", () => {
          expect(typeof btree.getKeys).toBe("function");
        });

        it("result is array", () => {
          const result = btree.getKeys();

          expect(result.constructor.name).toBe("Array");
        });

        it("result is array of numbers", () => {
          const result = btree.getKeys();

          expect(result[0]).toBe("15");
        });

        it("return expected result", () => {
          const arr = ["15", "30", "50"];

          const result = btree.getKeys();
          const it = arr[Symbol.iterator]();

          expect(result.length).toBe(3);

          for (const item of result) {
            const i = it.next().value;

            expect(item).toBe(i);
          }
        });

        it("return expected result for empty tree", () => {
          btree = new BTree(comparator);

          const result = btree.getKeys();

          expect(result.length).toBe(0);
        });
      });

      describe("getValues()", () => {
        it("method callable", () => {
          expect(typeof btree.getValues).toBe("function");
        });

        it("result is array", () => {
          const result = btree.getValues();

          expect(result.constructor.name).toBe("Array");
        });

        it("result is array of numbers", () => {
          const result = btree.getValues();

          expect(result[0]).toBe(150);
        });

        it("return expected result", () => {
          const arr = [150, 30, 50];

          const result = btree.getValues();
          const it = arr[Symbol.iterator]();

          expect(result.length).toBe(3);

          for (const item of result) {
            const i = it.next().value;

            expect(item).toBe(i);
          }
        });

        it("return expected result for empty tree", () => {
          btree = new BTree(comparator);

          const result = btree.getValues();

          expect(result.length).toBe(0);
        });
      });
    });
  });
});
