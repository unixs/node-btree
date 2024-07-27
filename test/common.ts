import { BTree, GLIB_VERSION } from "..";

export const MSG_TOO_FEW_ARGUMENTS = "Too few arguments.";

export function comparator(a: string, b: string) {
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

export function initBtree() {
  const btree = new BTree<string, number>(comparator);

  btree.set("50", 50);
  btree.set("30", 30);
  btree.set("15", 150);

  return btree;
}

export function createBTree() {
  const btree = new BTree(comparator);
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

  btree.set("50", 51);
  btree.set("15", 150);
  btree.set("30", 30);

  return { btree, check };
}

export function* initGenerator() {
  yield { key: "15", value: 150 };
  yield { key: "30", value: 30 };

  return { key: "50", value: 50 };
}

export function modernDescribe(describe: jest.Describe) {
  Object.defineProperty(describe, "modern", {
    value: GLIB_VERSION.hasGTreeNode() ? describe : describe.skip,
  });
}
