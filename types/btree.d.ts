/* eslint-disable @typescript-eslint/no-explicit-any */

export interface BTreeNodeObject<K, V> {
  key: K;
  value: V;
}

export type BTreeNodeVector<K, V> = [K, V];

/**
 * Base default interface of Binary tree
 */
export interface BinaryTree<K, V> extends Map<K, V>, Iterable<[K, V]> {
  get [Symbol.toStringTag](): string;
  // Other
  readonly height: number;

  // Map
  readonly size: number;
  clear(): void;
  delete(key: K): boolean;
  get(key?: K): V | undefined;
  has(key: K): boolean;
  set(key?: K, value?: V): this;
  forEach<T>(callbackfn: (value: V, key: K, btree: BinaryTree<K, V>, idx: number) => void, thisArg?: T): void;

  // Iterable
  [Symbol.iterator](): Iterator<BTreeNodeVector<K, V>>;
  entries(): IterableIterator<BTreeNodeVector<K, V>>;
  values(): IterableIterator<V>;
  keys(): IterableIterator<K>;

  map<T>(cbFn: (value: V, key: K, idx: number, btree: BinaryTree<K, V>) => T, thisArg?: any): T[];
  reduce<T>(cbFn: (prev: T, currVal: V, curKey: K, currIdx: number, btree: BinaryTree<K, V>) => T, thisArg?: any): T;
  reduce<T>(
    cbFn: (prev: T, currVal: V, curKey: K, currIdx: number, btree: BinaryTree<K, V>) => T,
    initialValue: T,
    thisArg?: any,
  ): T;

  filter<T>(cbFn: (v: V, k: K, idx: number) => boolean, thisArg?: T): BTree<K, V>;
}

/**
 * Extended interface of Binary tree
 */
export interface BinaryTreeExt<K, V> extends BinaryTree<K, V> {
  first(): BTreeNodeObject<K, V>;
  last(): BTreeNodeObject<K, V>;
  before(key: K, includeTaraget?: boolean): BinaryTreeExt<K, V>;
  after(key: K, includeTaraget?: boolean): BinaryTreeExt<K, V>;
  between(lowKey: K, highKey: K, includeTaraget?: boolean): BinaryTreeExt<K, V>;
  forEachReverse<T>(
    cbFn: (value: V, key: K, self: BinaryTreeExt<K, V>, idx: number, revIdx: number) => void,
    thisArg?: T,
  ): void;
  getKeys(): K[];
  getValues(): V[];
}

export interface BinaryTreeConversion<K, V> extends BinaryTree<K, V> {
  toMap(): Map<K, V>;
  toSet(): Set<V>;
  toArrays(): BTreeNodeVector<K, V>[];
  toArray(): BTreeNodeObject<K, V>[];
  flatten(): Array<K | V>;
}

/**
 * Direct-callable Binary tree constructor
 */
declare function BTree<K, V>(comparator: (a: K, b: K) => number): BTree<K, V>;

/**
 * Default implementation of Binary tree
 */
declare class BTree<K, V> implements BinaryTreeExt<K, V>, BinaryTreeConversion<K, V> {
  readonly size: number;
  readonly height: number;

  static from(comparator: (a: K, b: K) => number, source: Iterable<BTreeObjectNode<K, V>>): BTree<K, V>;

  constructor(comparator: (a: K, b: K) => number);

  get [Symbol.toStringTag](): string;

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

  forEach<T>(callbackfn: (value: V, key: K, btree: BTree<K, V>, idx: number) => void, thisArg?: T): void;
  forEachReverse<T>(
    cbFn: (value: V, key: K, btree: BTree<K, V>, idx: number, revIdx: number) => void,
    thisArg?: T,
  ): void;

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

/**
 * Information abot linked libGLib
 */
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
