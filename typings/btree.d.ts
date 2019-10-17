export interface BTreeComparator<K> {
  (a: K, b: K): number;
}

class BTree<K, V> extends Map<K, V> {
  public readonly height: number;
  public readonly size: number;
  public readonly comparator: BTreeComparator<K>

  constructor(comparator: BTreeComparator<K>, entries?: ReadonlyArray<readonly [K, V]> | null);

  set(key: K, value: V): this;
  get(key: K): V | undefined;
  delete(key: K): boolean;

  [Symbol.iterator](): IterableIterator<[K, V]>;

  forEach(callbackfn: (value: V, key: K, map: Map<K, V>) => void, thisArg?: any): void;
  entries(): IterableIterator<[K, V]>;
  keys(): IterableIterator<K>;
  values(): IterableIterator<V>;
  clear(): void;
  has(key: K): boolean;
}

export { BTree };

