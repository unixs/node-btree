#ifndef _CORE_H_
#define _CORE_H_

#include <node_api.h>
#include <glib.h>

#include <utils.h>
#include <type.h>
#include <comparator.h>

extern const char *msgTooFewArguments;
extern const char *msgCorrupt;

#define getEsGTreeNode(node) ((BTreeNode) g_tree_node_key(node))

void
nativeInsertNode(napi_env env, napi_value esBtree, napi_value box);
#define getNodeEsKey(env, node) getNodeEsField(env, node, KEY)
#define getNodeEsValue(env, node) getNodeEsField(env, node, VALUE)

#define CHECK_TREE_CORRUPT(env, node)                                          \
  if (node == NULL) {                                                          \
    NAPI_CALL(env, false,                                                      \
      napi_throw_error(env, NULL, msgCorrupt));                                \
                                                                               \
    return true;                                                               \
  }

napi_value
getEsUndef(napi_env env);

napi_value
getEsNull(napi_env env);

napi_value
getEsGlobal(napi_env env);

GPtrArray*
gtreeToPtrArray(GTree *gtree);

napi_value
unrefBtreeNodeEsObject(napi_env env, napi_ref ref, napi_value *key, napi_value *value);

napi_value
cloneBtreeWrapper(napi_env env, napi_value orig, BTree_t **btree);

napi_value
cloneInternalEsObject(napi_env env, napi_ref ref);

napi_ref
cloneInternalEsRef(napi_env env, napi_ref ref);


// Inline tools

inline napi_value
getEsConstructor(napi_env env) {
  extern napi_ref btreeConstructorRef;
  napi_value constructor;

  NAPI_CALL(env, true,
    napi_get_reference_value(env, btreeConstructorRef, &constructor));

  return constructor;
}

inline napi_value
extractEsComparator(napi_env env, BTree_t *btree) {
  napi_value comparator;

  g_assert_nonnull(btree->comparator);

  NAPI_CALL(env, false,
    napi_get_reference_value(env, btree->comparator, &comparator));

  return comparator;
}

inline gint
compareBTreeNodes(BTreeNode a, BTreeNode b, BTree_t *btree) {
  return nativeComparator((gconstpointer) a, (gconstpointer) b, (gpointer) btree);
}

#ifdef HAS_GTREE_NODE

GTreeNode*
btreeLookup(napi_env env, BTree_t *gtree, napi_value key);

napi_value
getNodeEsObject(napi_env env, GTreeNode *g_node);

napi_value
getNodeEsField(napi_env env, GTreeNode *g_node, const char *field);

napi_value
getNodeEsFieldPair(napi_env env, GTreeNode *g_node);


BTreeNode
cloneBTreeNode(napi_env env, BTree_t *btree, GTreeNode *node);

inline gint
compareGTreeNodes(GTreeNode *a, GTreeNode *b, BTree_t *btree) {
  return compareBTreeNodes(getEsGTreeNode(a), getEsGTreeNode(b), btree);
}

#endif // HAS_GTREE_NODE

#endif //_CORE_H_

