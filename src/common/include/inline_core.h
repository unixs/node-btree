#include <node_api.h>
#include <glib.h>

extern inline napi_value
getEsConstructor(napi_env env);

extern inline napi_value
extractEsComparator(napi_env env, BTree_t *btree);

extern inline gint
compareBTreeNodes(BTreeNode a, BTreeNode b, BTree_t *btree);

extern inline gint
compareGTreeNodes(GTreeNode *a, GTreeNode *b, BTree_t *btree);
