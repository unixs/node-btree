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

describe("Inheritance", () => {
  it("Create BTree instance", () => {
    const btree = new BTree(comparator);

    expect(btree).toBeInstanceOf(BTree);
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

    it('get method should throw exception without args', () => {
      const btree = initBtree();

      try {
        btree.get();
      }
      catch (e) {
        expect(e.message).toBe(MSG_TOO_FEW_ARGUMENTS);
      }
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

    it('Check set() method', () => {
      const btree = new BTree(comparator);

      btree.set("key", 10);
      btree.set(20, "value");

      expect(btree.get(20)).toBe("value");
      // expect(btree.get("key")).toBe(10);
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

      it('forEach() should throw error if few arguments', (done) => {
        const btree = initBtree();

        try {
          btree.forEach();

          done.fail("Should throw error.");
        }
        catch (e) {
          expect(e.message).toBe(MSG_TOO_FEW_ARGUMENTS);
          done();
        }
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
  it.todo("filter()");
  it.todo("toArray()");
  it.todo("toArrays()");
  it.todo("toObject()");
  it.todo("toJSON()");

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

    it('reduce() should throw error if few arguments', (done) => {
      const btree = initBtree();

      try {
        btree.reduce((acc, val) => val);

        done.fail("Should throw error.");
      }
      catch (e) {
        expect(e.message).toBe(MSG_TOO_FEW_ARGUMENTS);
        done();
      }
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

    it('map() should throw error if few arguments', (done) => {
      const btree = initBtree();

      try {
        btree.map();

        done.fail("Should throw error.");
      }
      catch (e) {
        expect(e.message).toBe(MSG_TOO_FEW_ARGUMENTS);
        done();
      }
    });

  });

});
