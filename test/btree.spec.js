const { BTree } = require("../lib");

const MSG_TOO_FEW_ARGUMENTS = "Too few arguments.";

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

  btree.set("50", 50);
  btree.set("30", 30);
  btree.set("15", 150);

  return btree;
}

function *initGenerator() {
  yield { key: "15", value: 150 };
  yield { key: "30", value: 30 };
  yield { key: "50", value: 50 };
}

describe("Inheritance", () => {
  it("Create BTree instance", () => {
    const btree = new BTree(comparator);

    expect(btree).toBeInstanceOf(BTree);
  });

  it("Create BTree instance without new", () => {
    const btree = BTree(comparator);

    expect(btree).toBeInstanceOf(BTree);
  });

  it('Constructor should throw error without arguments', () => {
    expect(() => new BTree()).toThrow(MSG_TOO_FEW_ARGUMENTS);
  });

  it('Constructor call should throw error without arguments', () => {
    expect(() => BTree()).toThrow(MSG_TOO_FEW_ARGUMENTS);
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

    it('from() should throw error without args', () => {
      expect(() => BTree.from()).toThrow(MSG_TOO_FEW_ARGUMENTS);
    });

    it('from() should throw error if comparator is bad', () => {
      expect(() => BTree.from([], []))
        .toThrow("First arg must be comparator qsort() like function");
    });

    it('from() should throw error if second arg is bad', () => {
      expect(() => BTree.from(comparator, ''))
        .toThrow("Second arg must be Array, Map or iterable");
    });

    it("from() key-value-able objects array", () => {
      const arr = [
        {
          key: 10,
          value: "100",
          a: "a"
        },
        {
          key: 20,
          value: "200",
          b: "b"
        },
        {
          key: 30,
          value: "300",
          c: "c"
        }
      ];

      const btree = BTree.from(comparator, arr);

      expect(btree.size).toBe(3);
      expect(btree.height).toBe(2);
      expect(btree.get(20)).toBe("200");
    });

    it('from() array of arrays [key, value]', () => {
      const arr = [
        [10, "10"],
        [30, "30"],
        [80, "80"],
        [20, "20"],
        [50, "50"]
      ];

      const btree = BTree.from(comparator, arr);

      expect(btree.size).toBe(5);
      expect(btree.height).toBe(3);
      expect(btree.get(30)).toBe("30");
    });

    it('from() mixed array of arrays & objects', () => {
      const arr = [
        [10, "10"],
        {
          key: 30,
          value: "30"
        },
        [80, "80"],
        {
          key: 20,
          value: "20"
        },
        [50, "50"]
      ];

      const btree = BTree.from(comparator, arr);

      expect(btree.size).toBe(5);
      expect(btree.height).toBe(3);
      expect(btree.get(30)).toBe("30");
      expect(btree.get(50)).toBe("50");
    });

    it('from() generator', () => {

      function* generator() {
        for (let i = 0; i < 1000; i++) {
          yield {
            key: i,
            value: `${i} bla-bla-bla`
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
    it.skip('from() Map instance', () => {
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

    it('Should have correct height field without nodes', () => {
      const btree = new BTree(comparator);

      expect(btree.height).toBe(0);
    });

    it("Should have correct heigth field with nodes", () => {
      const btree = initBtree();

      expect(btree.height).toBe(2);
    });

  });

  describe("Map() interface implementation", () => {

    it("Check size property if empty", () => {
      const btree = new BTree(comparator);

      expect(btree.size).toBe(0);
    });

    it("Check size property if has values", () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);
    });

    it('get method should be callable without args', () => {
      const btree = new BTree(comparator);

      expect(btree.get()).toBe(undefined);
    });

    it("Check get method (number => string)", () => {
      const btree = initBtree();

      expect(btree.get(15)).toBe(150);
    });

    it("Check get method (string => string)", () => {
      const btree = initBtree();

      expect(btree.get("15")).toBe(150);
    });

    it("Check get method (string => number)", () => {
      const btree = new BTree(comparator);

      btree.set(50, 50);
      btree.set(30, 30);
      btree.set(15, 150);

      expect(btree.get("15")).toBe(150);
    });
    it('Should be addable if key exists', () => {
      const btree = initBtree();

      btree.set("30", 40);

      expect(btree.get("30")).toBe(40);
    });

    it('Should accept set() without args', () => {
      const btree = new BTree(comparator);

      btree.set();

      expect(btree.size).toBe(1);
      expect(btree.get(undefined) === undefined).toBeTruthy();
      expect(btree.get() === undefined).toBeTruthy();
    });

    it('Should accept set() with one first arg', () => {
      const btree = new BTree(comparator);

      btree.set(1);

      expect(btree.size).toBe(1);
      expect(btree.get(1) === undefined).toBeTruthy();
    });

    it('Values should be replaceable by set()', () => {
      const btree = new BTree(comparator);

      btree.set(1, 2);
      btree.set(3, 4);
      btree.set(5, 6);

      expect(btree.size).toBe(3);
      expect(btree.get(3)).toBe(4);

      btree.set(3, 7);

      expect(btree.size).toBe(3);
      expect(btree.get(3)).toBe(7);
    });

    it('Items should be deletable if exists', () => {
      const btree = initBtree();

      const result = btree.delete("30");

      expect(result).toBe(true);
      expect(btree.size).toBe(2);
    });

    it('Items should be deletable if not exists', () => {
      const btree = initBtree();

      const result = btree.delete(31);

      expect(result).toBe(false);
      expect(btree.size).toBe(3);
    });

    it('Should be clearable by clear()', () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);

      btree.clear();

      expect(btree.size).toBe(0);
    });

    it('Should be processable after clear', () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);

      btree.clear();

      expect(btree.size).toBe(0);

      btree.set("One", 1);

      expect(btree.size).toBe(1);
    });

    it('has() method should be callable', () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);

      expect(btree.has("50")).toBe(true);
      expect(btree.has("100500")).toBe(false);
    });

    it('has() method should be callable without args', () => {
      const btree = initBtree();

      expect(btree.size).toBe(3);

      expect(btree.has()).toBe(true);
    });

    it('has() method should be callable without args and find undefined', () => {
      const btree = initBtree();

      btree.set();

      expect(btree.has()).toBe(true);
    });
  });

});

describe('Traverse functionality', () => {
  function createBTree() {
    const btree = new BTree(comparator);
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

    btree.set("50", 51);
    btree.set("15", 150);
    btree.set("30", 30);


    return { btree, check };
  }

  describe("Iterator interface", () => {

    it('Should have next method', () => {
      const btree = createBTree().btree;
      const iterator = btree[Symbol.iterator]();

      expect(typeof iterator.next).toBe('function');
    });

    it('Iterator should return value', () => {
      const { btree } = createBTree();
      const iterator = btree[Symbol.iterator]();

      expect(typeof iterator.next()).toBe('object');
    });

    it('Iterator result should have correct fields', () => {
      const { btree, check } = createBTree();
      const iterator = btree[Symbol.iterator]();

      const result = iterator.next();

      expect(result.value[0]).toBe(check[0].key);
      expect(result.value[1]).toBe(check[0].val);
      expect(result.done).toBe(false);
    });

  });

  describe("Iteration methods", () => {

    describe('forEach()', () => {
      it('Should be iterable by forEach()', () => {
        const btree = new BTree(comparator);

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

        btree.forEach((val, key) => {
          const check = checkIterator.next().value;

          expect(key).toBe(check.key);
          expect(val).toBe(check.val);
        });
      });

      it('forEach() should have value first arg', () => {
        const btree = initBtree();

        const check = [150, 30, 50];

        const checkIterator = check[Symbol.iterator]();

        btree.forEach((val) => {
          expect(val).toBe(checkIterator.next().value);
        });
      });

      it('forEach() should have key second arg', () => {
        const btree = initBtree();

        const check = ["15", "30", "50"];

        const checkIterator = check[Symbol.iterator]();

        btree.forEach((_val, key) => {
          expect(key).toBe(checkIterator.next().value);
        });
      });

      it("forEach() callback should have third idx arg", () => {
        const btree = initBtree();

        let i = 0;

        btree.forEach((_val, _key, idx) => {
          expect(idx).toBe(i++);
        });
      });

      it("forEach() should recive context with basic function", () => {
        const btree = initBtree();

        btree.forEach(function(val) {

          expect(this.something).toBe("test");

          return val;
        }, { something: "test" });
      });

      it("forEach() should not recive context with arrow function", () => {
        const btree = initBtree();

        btree.forEach((val) => {

          expect(this).toMatchObject({});
          expect(this.test).toBe(undefined);

          return val;
        }, { test: "test" });
      });

      it("forEach() should have Object context with arrow function", () => {
        const btree = initBtree();

        btree.forEach((val) => {

          expect(this).toMatchObject({});

          return val;
        });
      });

      it("forEach() should have global context with basic function", () => {
        const btree = initBtree();

        btree.forEach(function(val) {
          expect(this.process).toBeDefined();
          expect(this.process.nextTick).toBeDefined();
          expect(this.setTimeout).toBeDefined();

          return val;
        });
      });

      it('forEach() should throw error if few arguments', () => {
        const btree = initBtree();

        expect(() => btree.forEach()).toThrow(MSG_TOO_FEW_ARGUMENTS);
      });

    });


    it('Should be iterable by for-of', () => {
      const btree = new BTree(comparator);

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

    it('Should be iterable by entries()', () => {
      const btree = new BTree(comparator);

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

    it('Should be iterable by values()', () => {
      const btree = new BTree(comparator);

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

    it('Should be iterable by keys()', () => {
      const btree = new BTree(comparator);

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

    it('Should be iterable if empty', () => {
      const btree = new BTree(comparator);

      const result = btree.entries().next();

      expect(result.value).toBe(undefined);
      expect(result.done).toBe(true);
    });

  });

});

describe('Extra methods', () => {
  it.todo("toArray()");
  it.todo("toArrays()");
  it.todo("toJSON()");
  it.todo("toSet()");
  it.todo("toMap()");

  describe("filter()", () => {

    it("filter() should be callable & return BTree", () => {
      const btree = initBtree();

      expect(btree.filter.constructor.name).toBe("Function");
    });

    it("filter() should return BTree", () => {
      const btree = initBtree();

      expect(btree.filter(() => true).constructor.name)
        .toBe("BTree");
    });

    it("filter() should return all items", () => {
      const btree = initBtree();

      expect(btree.filter(() => true).size)
        .toBe(3);
    });

    it("filter() should return 0 items", () => {
      const btree = initBtree();

      expect(btree.filter(() => false).size)
        .toBe(0);
    });

    it("filter() callback should have first value arg", () => {
      const btree = initBtree();

      const it = initGenerator();

      btree.filter((val) => {
        expect(val).toBe(it.next().value.value);
      });
    });

    it("filter() callback should have second key arg", () => {
      const btree = initBtree();

      const it = initGenerator();

      btree.filter((_val, key) => {
        expect(key).toBe(it.next().value.key);
      });
    });

    it("filter() callback should have third idx arg", () => {
      const btree = initBtree();

      let i = 0;

      btree.filter((_val, _key, idx) => {
        expect(idx).toBe(i++);
      });
    });
  });

  describe('reduce()', () => {
    it("reduce() should be callable & return valid value for numbers", () => {
      const btree = initBtree();

      const result = btree.reduce((acc, val, _key, _idx, _btree) => {
        return acc + val;
      }, 3);

      expect(result).toBe(233);
    });

    it("reduce() callback should have first value arg", () => {
      const btree = initBtree();

      const array = btree.reduce((acc, val) => {
        acc.push(val);

        return acc;
      }, []);

      expect(array.length).toBe(3);
      expect(array[2]).toBe(50);
    });

    it("reduce() callback should have second key arg", () => {
      const btree = initBtree();

      const str = btree.reduce((acc, _val, key) => {
        return acc + key;
      }, "");

      expect(str.length).toBe(6);
      expect(str).toBe("153050");
    });

    it("reduce() callback should have third idx arg", () => {
      const btree = initBtree();

      const array = btree.reduce((acc, _val, _key, idx) => {
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

      const result = btree.reduce((_acc, _val, _key, _idx, btree) => btree, null);

      expect(result).toBeInstanceOf(BTree);
      expect(result.size).toBe(3);
      expect(result.get("15")).toBe(150);
    });

    it('reduce() should throw error if few arguments', () => {
      const btree = initBtree();


      expect(() => btree.reduce((acc, val) => val)).toThrow(MSG_TOO_FEW_ARGUMENTS);
    });

  });


  describe('map()', () => {

    it("map() should be callable & return array", () => {
      const btree = initBtree();

      const array = btree.map(val => val);

      expect(array.constructor.name).toBe("Array");
      expect(array.length).toBe(3);
    });

    it("map() callback should have first value arg", () => {
      const btree = initBtree();

      const array = btree.map(val => val);

      expect(array[0]).toBe(150);
      expect(array.length).toBe(3);
    });

    it("map() callback should have second key arg", () => {
      const btree = initBtree();

      const array = btree.map((_val, key) => key);

      expect(array[1]).toBe("30");
      expect(array.length).toBe(3);
    });

    it("map() callback should have third idx arg", () => {
      const btree = initBtree();

      const array = btree.map((_val, _key, idx) => idx);

      expect(array.length).toBe(3);

      for (let i = 0; i < 3; i++) {
        expect(array[i]).toBe(i);
      }
    });

    it("map() callback should have fourth bTree arg", () => {
      const btree = initBtree();

      const array = btree.map((_val, _key, _idx, btree) => btree);

      expect(array[0]).toBeInstanceOf(BTree);
      expect(array.length).toBe(3);
      expect(array[1].get("15")).toBe(150);
    });

    it("map() should recive context with basic function", () => {
      const btree = initBtree();

      btree.map(function(val) {

        expect(this.test).toBe("test");

        return val;
      }, { test: "test" });
    });

    it("map() should not recive context with arrow function", () => {
      const btree = initBtree();

      btree.map((val) => {

        expect(this).toMatchObject({});
        expect(this.test).toBe(undefined);

        return val;
      }, { test: "test" });
    });

    it("map() should have Object context with arrow function", () => {
      const btree = initBtree();

      btree.map((val) => {

        expect(this).toMatchObject({});

        return val;
      });
    });

    it("map() should have global context with basic function", () => {
      const btree = initBtree();

      btree.map(function(val) {
        expect(this.process).toBeDefined();
        expect(this.process.nextTick).toBeDefined();
        expect(this.setTimeout).toBeDefined();

        return val;
      });
    });

    it('map() should throw error if few arguments', () => {
      const btree = initBtree();

      expect(() => btree.map()).toThrow(MSG_TOO_FEW_ARGUMENTS);
    });

  });

});
