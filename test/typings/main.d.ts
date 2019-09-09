declare module "*/node_btree" {

  interface BTreeComarator<K> {
    (a: K, b: K): number;
  }

  class BTree<K, V> {
    public height: number;
    readonly public comparator: BTreeComarator<K>

    constructor(comparator: BTreeComarator<K>);

    set(key: K, value: V): BTree<K, V>;
    get(key: K): V | undefined;
    size(): number;

    [Symbol.iterator](): IterableIterator<[K, V]>;
    entries(): IterableIterator<[K, V]>;
    keys(): IterableIterator<K>;
    values(): IterableIterator<V>;
  }

  export {
    BTree
  };
}
