export interface BTreeObjectNode<K, V> {
  key: K;
  value: V;
}

declare class BTree<K, V> implements Map<K, V> {
  readonly size: number;
  readonly height: number;

  // static from(it: Iterable<BTreeObjectNode<K, V>>): BTree<K, V>;

  constructor(comparator: (a: K, B: K) => number);

  forEach(cbFn: (value: V, key: K, map: BTree<K, V>) => void, thisArg?: any): void;
  clear(): void;
  delete(key: K): boolean;
  get(key: K): V | undefined;
  has(key: K): boolean;
  set(key: K, value: V): this;

  map<T>(cbFn: (value: V, key: K, idx: number, btree: BTree<K, V>) => T, thisArg?: any): T[];
  reduce<T>(): T;
  filter(): BTree<K, V>;

  forEachReverse(cbFn: (value: V, key: K, map: BTree<K, V>) => void, thisArg?: any): void;

  toMap(): Map<K, V>;
  toSet();
  toArrays(): K|V[][];
  toArray(): BTreeObjectNode<K, V>[];
  flatten(): K|V[];
  getKeys(): K[];
  getValues(): V[];
  first(): BTreeObjectNode<K, V>;
  last(): BTreeObjectNode<K, V>;
  before(key: K): BTree<K, V>;
  after(key: K): BTree<K, V>;
  between(lowKey: K, highKey: K): BTree<K, V>;

  [Symbol.iterator](): IterableIterator<[K, V]>;
  entries(): IterableIterator<[K, V]>;
  values(): IterableIterator<V>;
  keys(): IterableIterator<K>;
}

declare namespace GLIB_VERSION {
  const MAJOR: number;
  const MINOR: number;
  const PATCH: number;

  function hasGTreeNode(): boolean;
}

export { BTree, GLIB_VERSION };
