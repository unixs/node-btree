#include <core.h>

#ifndef _TYPE_H_
#define _TYPE_H_

/**
 * Context for native bTree
 */
typedef struct
{
  // Node runtime environment
  napi_env env;
  //  ES comparator passed form constructor
  napi_ref comparator;
  // GLib Binary Tree
  GTree *nativeTree;
} BTree_t;

/**
 * bTree Node
 *
 * Save pointer to self tree & kay-value pair ES Object
 */
typedef struct {
  // Ref to ES value
  napi_ref esKeyValue;
  // Pointer to self tree
  BTree_t *bTree;
} BTreeNode_t;

// Type alias for tree node
typedef BTreeNode_t* BTreeNode;

BTree_t*
allocNativeBTree(napi_env env);

void
freeNativeBTree(napi_env env, void *finalize_data, void *finalize_hint);

void
reallocGTree(BTree_t *bTree);

#endif // _TYPE_H_
