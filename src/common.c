#include <common.h>

extern const char *msgTooFewArguments = "Too few arguments.";
extern const char *msgCorrupt = "BTree corrupt. Next node expected but it is null.";

void nativeInsertNode(napi_env env, napi_value esBtree, napi_value box) {
  BTree_t *bTree;

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esBtree, (void **) &bTree));

  // Set ref counter to 1 for protect from GC
  napi_ref nodeRef;
  NAPI_CALL(env, false,
    napi_create_reference(env, box, 1, &nodeRef));

  // Alloc new tree node
  BTreeNode node;
  NEW_NODE(node, bTree, nodeRef);

  // Add es plain object to native bTree
  g_tree_replace(bTree->nativeTree, node, node);
}
