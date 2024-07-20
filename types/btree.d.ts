/* eslint-disable @typescript-eslint/no-explicit-any */

export interface BTreeNodeObject<K, V> {
  key: K;
  value: V;
}

export type BTreeNodeVector<K, V> = [K, V];

declare function BTree<K, V>(comparator: (a: K, b: K) => number): BTree<K, V>;

declare class BTree<K, V> implements Map<K, V>, Iterable<[K, V]> {
  readonly size: number;
  readonly height: number;

  static from(comparator: (a: K, b: K) => number, source: Iterable<BTreeObjectNode<K, V>>): BTree<K, V>;

  constructor(comparator: (a: K, b: K) => number);

  clear(): void;
  delete(key: K): boolean;
  get(key?: K): V | undefined;
  has(key: K): boolean;
  set(key?: K, value?: V): this;

  map<T>(cbFn: (value: V, key: K, idx: number, btree: BTree<K, V>) => T, thisArg?: any): T[];
  reduce<T>(cbFn: (prev: T, currVal: V, curKey: K, currIdx: number, btree: BTree<K, V>) => T, thisArg?: any): T;
  reduce<T>(
    cbFn: (prev: T, currVal: V, curKey: K, currIdx: number, btree: BTree<K, V>) => T,
    initialValue: T,
    thisArg?: any,
  ): T;
  filter(cbFn: (v: V, k: K, idx: number) => boolean, thisArg?: any): BTree<K, V>;

  forEach(cbFn: (value: V, key: K, idx: number) => void, thisArg?: any): void;
  forEachReverse(cbFn: (value: V, key: K, idx: number, revIdx: number) => void, thisArg?: any): void;

  toMap(): Map<K, V>;
  toSet(): Set<V>;
  toArrays(): BTreeNodeVector<K, V>[];
  toArray(): BTreeNodeObject<K, V>[];
  flatten(): Array<K | V>;
  getKeys(): K[];
  getValues(): V[];
  first(): BTreeNodeObject<K, V>;
  last(): BTreeNodeObject<K, V>;
  before(key: K, includeTaraget?: boolean): BTree<K, V>;
  after(key: K, includeTaraget?: boolean): BTree<K, V>;
  between(lowKey: K, highKey: K, includeTaraget?: boolean): BTree<K, V>;

  [Symbol.iterator](): Iterator<BTreeNodeVector<K, V>>;
  entries(): IterableIterator<BTreeNodeVector<K, V>>;
  values(): IterableIterator<V>;
  keys(): IterableIterator<K>;
}

declare namespace GLIB_VERSION {
  const MAJOR: number;
  const MINOR: number;
  const PATCH: number;

  /**
   * Returns true if libglib2 version >= 2.68
   */
  function hasGTreeNode(): boolean;
}

export { BTree, GLIB_VERSION };
