#include <node_api.h>
#include <glib.h>

#ifndef _COMMON_H_
#define _COMMON_H_

#include "utils.h"


extern const char *msgTooFewArguments;
extern const char *msgCorrupt;

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


// Cached ES constructor
extern napi_ref constructor;

void nativeInsertNode(napi_env env, napi_value esBtree, napi_value box);

#endif //_COMMON_H_
