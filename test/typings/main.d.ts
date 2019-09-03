declare module "*/node_btree.node" {

  interface BTreeComarator<K> {
    (a: K, b: K): number;
  }

  class BTree<K, V> {
    height: number;

    constructor(comparator: BTreeComarator<K>);

    set(key: K, value: V): BTree<K, V>;
    get(key: K): V | undefined;
    size(): number;
  }

  export {
    BTree
  };
}
