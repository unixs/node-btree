#include <node_api.h>
#include <glib.h>

#include "include/common.h"
#include "include/glib_local.h"

static const char *msgTooFewArguments = "Too few arguments.";
static const char *msgCorrupt = "BTree corrupt. Next node expected but it is null.";

/**
 * Native iterator state
 */
typedef enum ITERATOR_STATE {
  ITERATOR_INIT,
  ITERATOR_LOOP,
  ITERATOR_END
} IteratorState_t;

/**
 * Context for native bTree
 */
typedef struct {
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

/**
 * ES iterator native context
 */
typedef struct {
  // Pointer to self tree
  BTree_t *bTree;
  // Saved iterator state
  IteratorState_t state;
  // Internal GLib BTree node
  GTreeNode_t currentNode;
  // ES value for return
  napi_value value;
} IteratorContext_t;

typedef struct {
  napi_value esbTree;
  napi_value callback;
  napi_value cbThis;
  size_t idx;
  BTree_t *bTree;
  void *data;
} ForEachContext_t;

/**
 * Native callback for generic operations
 */
typedef void (*iteratorResultCallback)(IteratorContext_t *ctxt);

// Cached ES constructor
static napi_ref constructor;

static void nativeInsertNode(napi_env env, napi_value esBtree, napi_value box) {
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

/**
 * Callback for iterator wich return key with value
 */
static inline void iteratorResultDefaultCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value esValue, tmp;

  BTreeNode node = (BTreeNode) local_g_tree_node_value(ctxt->currentNode);

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esValue));

  NAPI_CALL(env, false,
    napi_create_array_with_length(env, 2, &ctxt->value));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, KEY, &tmp));
  NAPI_CALL(env, false,
    napi_set_element(env, ctxt->value, 0, tmp));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, VALUE, &tmp));
  NAPI_CALL(env, false,
    napi_set_element(env, ctxt->value, 1, tmp));
}

/**
 * Callback for iterator wich return key only
 */
static inline void iteratorResultKeyCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value esValue;

  BTreeNode node = (BTreeNode) local_g_tree_node_value(ctxt->currentNode);

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esValue));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, KEY, &ctxt->value));
}

/**
 * Callback for iterator wich return value only
 */
static inline void iteratorResultValueCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value esValue;

  BTreeNode node = (BTreeNode) local_g_tree_node_value(ctxt->currentNode);

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esValue));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, VALUE, &ctxt->value));
}

/**
 * Unref all bTree nodes for GC access.
 */
static inline gboolean removeTreeNode(gpointer key, gpointer val, gpointer data) {
  BTree_t *bTree = (BTree_t *) data;

  // Remove es value from native bTree
  g_tree_remove(bTree->nativeTree, (BTreeNode) key);

  return FALSE;
}

/**
 * Free allocated GTree & unref comparator for GC access
 */
static void freeNativeBTree(napi_env env, void *finalize_data, void *finalize_hint) {
  BTree_t *bTree = (BTree_t *) finalize_data;

  // Unref comparator function for GC access
  NAPI_CALL(env, false,
    napi_delete_reference(env, bTree->comparator));

  // Destroy native bTree & release memory
  g_tree_destroy(bTree->nativeTree);

  // Release BTree_t struct memory
  g_free((gpointer) bTree);
}

/**
 * Free iterator native data
 */
static void freeIterator(napi_env env, void *finalize_data, void *finalize_hint) {
  g_free((gpointer) finalize_data);
}

/**
 * Free tree value
 */
static void freeTreeValue(gpointer treeValue) {
  FREE_NODE(treeValue);
}

/**
 * Native callback for ES map()
 */
static gboolean nativeBTreeMap(gpointer key, gpointer value, gpointer data) {
  BTreeNode node = (BTreeNode) value;
  ForEachContext_t *ctxt = (ForEachContext_t *) data;
  napi_value array = (napi_value) ctxt->data;
  napi_env env = ctxt->bTree->env;

  napi_value esNode, esKey, esValue, esIdx, cbResult;

  if (node == NULL) {
    NAPI_CALL(env, false,
      napi_throw_error(env, NULL, msgCorrupt));

    return true;
  }

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esNode));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esNode, KEY, &esKey));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esNode, VALUE, &esValue));

  NAPI_CALL(env, false,
    napi_create_int64(env, ctxt->idx, &esIdx));

  napi_value argv[] = {
    esValue,
    esKey,
    esIdx,
    ctxt->esbTree
  };

  NAPI_CALL(env, false,
    napi_call_function(env, ctxt->cbThis, ctxt->callback,
      (sizeof(argv) / sizeof(napi_value)), argv, &cbResult));

  NAPI_CALL(env, false,
    napi_set_element(env, array, ctxt->idx++, cbResult));

  return false;
}

/**
 * Native callback for es reduce()
 */
static gboolean nativeBTreeReduce(gpointer key, gpointer value, gpointer data) {
  BTreeNode node = (BTreeNode) value;
  ForEachContext_t *ctxt = (ForEachContext_t *) data;
  napi_value accumulator = (napi_value) ctxt->data;
  napi_env env = ctxt->bTree->env;

  napi_value esNode, esKey, esValue, esIdx, cbResult;

  if (node == NULL) {
    NAPI_CALL(env, false,
      napi_throw_error(env, NULL, msgCorrupt));

    return true;
  }

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esNode));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esNode, KEY, &esKey));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esNode, VALUE, &esValue));

  NAPI_CALL(env, false,
    napi_create_int64(env, ctxt->idx++, &esIdx));

  napi_value argv[] = {
    accumulator,
    esValue,
    esKey,
    esIdx,
    ctxt->esbTree
  };

  NAPI_CALL(env, false,
    napi_call_function(env, ctxt->cbThis, ctxt->callback,
      (sizeof(argv) / sizeof(napi_value)), argv, &cbResult));

  ctxt->data = (void *) cbResult;

  return false;
}

static gboolean nativeBTreeFilter(gpointer key, gpointer value, gpointer data) {
  BTreeNode node = (BTreeNode) value;
  ForEachContext_t *ctxt = (ForEachContext_t *) data;
  napi_value accumulator = (napi_value) ctxt->data;
  napi_env env = ctxt->bTree->env;

  napi_value esNode, esKey, esValue, esIdx, cbResult;

  if (node == NULL) {
    NAPI_CALL(env, false,
      napi_throw_error(env, NULL, msgCorrupt));

    return true;
  }

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esNode));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esNode, KEY, &esKey));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esNode, VALUE, &esValue));

  NAPI_CALL(env, false,
    napi_create_int64(env, ctxt->idx++, &esIdx));

  napi_value argv[] = {
    esValue,
    esKey,
    esIdx,
    ctxt->esbTree
  };

  NAPI_CALL(env, false,
    napi_call_function(env, ctxt->cbThis, ctxt->callback,
      (sizeof(argv) / sizeof(napi_value)), argv, &cbResult));

  NAPI_CALL(env, true,
    napi_coerce_to_bool(env, cbResult, &cbResult));

  bool ok = false;

  NAPI_CALL(env, true,
    napi_get_value_bool(env, cbResult, &ok));

  if (ok) {
    napi_handle_scope scope;

    NAPI_CALL(env, true,
      napi_open_handle_scope(env, &scope));

    napi_value box;

    // Create box es object
    NAPI_CALL(env, false,
      napi_create_object(env, &box));

    // Set key & value to box
    NAPI_CALL(env, false,
      napi_set_named_property(env, box, KEY, esKey));
    NAPI_CALL(env, false,
      napi_set_named_property(env, box, VALUE, esValue));

    nativeInsertNode(env, accumulator, box);

    NAPI_CALL(env, true,
      napi_close_handle_scope(env, scope));
    // ctxt->data = (void *) cbResult;
  }

  return false;
}

/**
 * Native comparator function
 */
static inline gint nativeComparator(gconstpointer a, gconstpointer b, gpointer bTree) {
  int64_t compareResult = 0;

  BTree_t *bTreeWrap = (BTree_t *) bTree;
  BTreeNode nodeA = (BTreeNode) a;
  BTreeNode nodeB = (BTreeNode) b;

  napi_env env = bTreeWrap->env;
  napi_value boxA, boxB, keyA, keyB, esNull, esResult, comparator;

  NAPI_CALL(env, false,
    napi_get_reference_value(env, nodeA->esKeyValue, &boxA));

  NAPI_CALL(env, false,
    napi_get_reference_value(env, nodeB->esKeyValue, &boxB));

  NAPI_CALL(env, false,
    napi_get_named_property(env, boxA, KEY, &keyA));

  NAPI_CALL(env, false,
    napi_get_named_property(env, boxB, KEY, &keyB));

  NAPI_CALL(env, false,
    napi_get_null(env, &esNull));

  NAPI_CALL(env, false,
    napi_get_reference_value(env, bTreeWrap->comparator, &comparator));

  napi_value argv[] = { keyA, keyB };
  NAPI_CALL(env, false,
    napi_call_function(env, esNull, comparator, 2, argv, &esResult));

  NAPI_CALL(env, false,
    napi_get_value_int64(env, esResult, &compareResult));

  return (gint) compareResult;
}

/**
 * ES callback. Return bTree height
 */
static napi_value esHeight(napi_env env, napi_callback_info cbInfo) {
  napi_value esHeight;
  napi_value esThis;
  BTree_t *bTree;

  // Get es this
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Native call to glib tree
  gint nativeHeight = g_tree_height(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env, false,
    napi_create_int64(env, nativeHeight, &esHeight));

  return esHeight;
}

/**
 * ES callback. Return bTree size (nodes count).
 */
static napi_value esSize(napi_env env, napi_callback_info cbInfo) {
  napi_value esSize;
  napi_value esThis;
  BTree_t *bTree;

  // Get es this
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Native call to glib tree
  gint nativeSize = g_tree_nnodes(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env, false,
    napi_create_int64(env, (int64_t) nativeSize, &esSize));

  return esSize;
}

/**
 * ES callback. Delete node from bTree by key
 */
static napi_value esDelete(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, result, searchBox;
  napi_ref searchBoxRef;
  BTree_t *bTree;

  size_t argc = 1;
  napi_value argv[1];

  // Get es this
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // If key arg not passed. Set it to undefined
  if (argc < 1) {
    NAPI_CALL(env, false,
      napi_get_undefined(env, &argv[0]));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Create es search box object for comparing with stored in bTree boxes
  NAPI_CALL(env, false,
    napi_create_object(env, &searchBox));

  NAPI_CALL(env, false,
    napi_set_named_property(env, searchBox, KEY, argv[0]));

  NAPI_CALL(env, false,
    napi_create_reference(env, searchBox, 0, &searchBoxRef));

  BTreeNode_t searchNode = {
    searchBoxRef,
    NULL
  };

  gboolean found = g_tree_remove(bTree->nativeTree, &searchNode);

  NAPI_CALL(env, false,
    napi_get_boolean(env, found, &result));

  return result;
}

/**
 * ES callback. Add element to bTree.
 */
static napi_value esSet(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  size_t  argc = 2,
          expectedArgc = argc;
  napi_value argv[2], key, value, box;

  // Get es this
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // Set to undefined all (key, value) missing arguments.
  for (size_t i = argc; i < expectedArgc; i++) {
    NAPI_CALL(env, false,
      napi_get_undefined(env, &argv[i]));
  }

  key = argv[0];
  value = argv[1];

  // Create box es object
  NAPI_CALL(env, false,
    napi_create_object(env, &box));

  // Set key & value to box
  NAPI_CALL(env, false,
    napi_set_named_property(env, box, KEY, key));
  NAPI_CALL(env, false,
    napi_set_named_property(env, box, VALUE, value));

  nativeInsertNode(env, esThis, box);

  return esThis;
}

/**
 * ES callback. Delete all nodes from bTree
 */
static napi_value esClear(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTree_t *bTree;

  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  g_tree_foreach(bTree->nativeTree, removeTreeNode, bTree);

  napi_value result;
  NAPI_CALL(env, false,
    napi_get_undefined(env, &result));

  return result;
}

/**
 * ES callback. Check key in bTree.
 */
static napi_value esHas(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  napi_ref boxRef;
  BTree_t *bTree;
  size_t argc = 1;
  napi_value argv[1];

  napi_value box;

  // Get es this
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // If key arg not passed. Set it to undefined
  if (argc < 1) {
    NAPI_CALL(env, false, napi_get_undefined(env, &argv[0]));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  NAPI_CALL(env, false,
    napi_create_object(env, &box));

  NAPI_CALL(env, false,
    napi_set_named_property(env, box, KEY, argv[0]));

  NAPI_CALL(env, false,
    napi_create_reference(env, box, 0, &boxRef));

  BTreeNode_t searchNode = {
    boxRef,
    NULL
  };

  gpointer found = g_tree_lookup(bTree->nativeTree, &searchNode);

  napi_value result;

  if (found == NULL) {
    NAPI_CALL(env, false,
      napi_get_boolean(env, false, &result));
  }
  else {
    NAPI_CALL(env, false,
      napi_get_boolean(env, true, &result));
  }

  return result;
}

/**
 * ES callback. Return value from bTree by key
 */
static napi_value esGet(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  napi_value result;
  BTree_t *bTree;
  size_t argc = 1;
  napi_value argv[1];

  // Get es this
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // If key arg not passed. Set it to undefined
  if (argc < 1) {
    NAPI_CALL(env, false,
      napi_get_undefined(env, &argv[0]));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  napi_value key = argv[0];
  napi_value lookupBox;
  napi_ref lookupRef;

  NAPI_CALL(env, false,
    napi_create_object(env, &lookupBox));

  NAPI_CALL(env, false,
    napi_set_named_property(env, lookupBox, KEY, key));

  NAPI_CALL(env, false,
    napi_create_reference(env, lookupBox, 0, &lookupRef));

  BTreeNode_t lookupNode = {
    lookupRef,
    NULL
  };

  // Native call to glib tree
  BTreeNode lookupResult =
    (BTreeNode) g_tree_lookup(bTree->nativeTree, &lookupNode);

  if (lookupResult == NULL) {
    NAPI_CALL(env, false,
      napi_get_undefined(env, &result));
  }
  else {
    NAPI_CALL(env, false,
      napi_get_reference_value(env, lookupResult->esKeyValue, &result));

    NAPI_CALL(env, false,
      napi_get_named_property(env, result, VALUE, &result));
  }

  return result;
}

/**
 * ES callback. Iterator next() method.
 */
static napi_value esIteratorNext(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  IteratorContext_t *itCtxt;
  iteratorResultCallback resultCb;

  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, (void **) &resultCb));

  // Extract native pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &itCtxt));

  switch (itCtxt->state) {
    case ITERATOR_INIT:
      itCtxt->state = ITERATOR_LOOP;
      itCtxt->currentNode = local_g_tree_first_node(itCtxt->bTree->nativeTree);
      break;

    case ITERATOR_LOOP:
      itCtxt->currentNode = local_g_tree_node_next(itCtxt->currentNode);
      break;

    case ITERATOR_END:
      // noop
      break;
  }

  if (itCtxt->currentNode == NULL) {
    itCtxt->state = ITERATOR_END;
  }

  // Prepare result:

  napi_value esIteratorResult, isDone;
  NAPI_CALL(env, false,
    napi_create_object(env, &esIteratorResult));

  if (itCtxt->state == ITERATOR_END) {
    NAPI_CALL(env, false,
      napi_get_undefined(env, &itCtxt->value));

    NAPI_CALL(env, false,
      napi_get_boolean(env, true, &isDone));
  }
  else {
    resultCb(itCtxt);
    NAPI_CALL(env, false,
      napi_get_boolean(env, false, &isDone));
  }

  NAPI_CALL(env, false,
    napi_set_named_property(env, esIteratorResult, VALUE, itCtxt->value));

  NAPI_CALL(env, false,
    napi_set_named_property(env, esIteratorResult, "done", isDone));

  return esIteratorResult;
}

/**
 * ES callback. bTree generator function.
 */
static napi_value esGenerator(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, esIterator;
  BTree_t *bTree;
  void *data;

  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, &data));

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Create es Iterator
  NAPI_CALL(env, false,
    napi_create_object(env, &esIterator));

  // Create next() iterator method
  napi_value nextFunction;
  NAPI_CALL(env, false,
    napi_create_function(env, "next", NAPI_AUTO_LENGTH, esIteratorNext, data, &nextFunction));
  NAPI_CALL(env, false,
    napi_set_named_property(env, esIterator, "next", nextFunction));

  // Alloc memory for native iterator context
  IteratorContext_t *itCtxt = g_new(IteratorContext_t, 1);
  itCtxt->bTree = bTree;
  itCtxt->state = ITERATOR_INIT;

  // Attach native data (context) to es value (iterator)
  napi_ref ref;
  NAPI_CALL(env, false,
    napi_wrap(env, esIterator, (void *) itCtxt, freeIterator, NULL, &ref));

  return esIterator;
}

/**
 * Native forEach() callback
 */
static gboolean nativeBTreeForEach(gpointer key, gpointer val, gpointer data) {
  BTreeNode node = (BTreeNode) val;
  ForEachContext_t *ctxt = (ForEachContext_t *) data;
  napi_env env = ctxt->bTree->env;
  napi_value esObject, esKey, esValue, esIdx, esNull;

  if (val == NULL) {
    NAPI_CALL(env, false,
      napi_throw_error(env, NULL, msgCorrupt));

    return TRUE;
  }

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esObject));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esObject, KEY, &esKey));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esObject, VALUE, &esValue));

  NAPI_CALL(env, false,
    napi_create_int64(env, ctxt->idx, &esIdx));

  napi_value argv[] = {
    esValue,
    esKey,
    esIdx
  };

  NAPI_CALL(env, false,
    napi_get_null(env, &esNull));

  NAPI_CALL(env, false,
    napi_call_function(env, ctxt->cbThis, ctxt->callback, (sizeof(argv) / sizeof(napi_value)), argv, NULL));

  ctxt->idx++;

  return FALSE;
}

/**
 * ES callback. es forEach() method
 */
static napi_value esForeach(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, undefined, callback, cbThis, argv[2];
  BTree_t *bTree;
  size_t argc = 2;


  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  CHECK_ARGC(1, msgTooFewArguments);
  callback = argv[0];

  if (argc > 1) {
    cbThis = argv[1];
  }
  else {
    NAPI_CALL(env, true,
      napi_get_global(env, &cbThis));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  ForEachContext_t ctxt = {
    esThis,
    callback,
    cbThis,
    0,
    bTree,
    NULL
  };

  g_tree_foreach(bTree->nativeTree, nativeBTreeForEach, &ctxt);

  NAPI_CALL(env, false,
    napi_get_undefined(env, &undefined));

  return undefined;
}

static napi_value esMap(napi_value env, napi_callback_info cbInfo) {
  napi_value esThis, array, callback, cbThis, argv[2];
  BTree_t *bTree;
  size_t argc = 2;

  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // Extract native BTree pointer
  EXTRACT_BTREE(env, esThis, bTree);

  CHECK_ARGC(1, msgTooFewArguments);
  callback = argv[0];

  if (argc > 1) {
    cbThis = argv[1];
  }
  else {
    NAPI_CALL(env, true,
      napi_get_global(env, &cbThis));
  }

  gint bTreeSize = g_tree_nnodes(bTree->nativeTree);

  NAPI_CALL(env, true,
    napi_create_array_with_length(env, (size_t) bTreeSize, &array));

  ForEachContext_t ctxt = {
    esThis,
    callback,
    cbThis,
    0,
    bTree,
    array
  };

  g_tree_foreach(bTree->nativeTree, nativeBTreeMap, &ctxt);

  return array;
}

static napi_value esReduce(napi_value env, napi_callback_info cbInfo) {
  napi_value esThis, array, callback, accumulator, cbThis, argv[2];
  BTree_t *bTree;
  size_t argc = 2;

  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // Extract native BTree pointer
  EXTRACT_BTREE(env, esThis, bTree);

  CHECK_ARGC(2, msgTooFewArguments);
  callback = argv[0];
  accumulator = argv[1];

  NAPI_CALL(env, true,
    napi_get_global(env, &cbThis));

  ForEachContext_t ctxt = {
    esThis,
    callback,
    cbThis,
    0,
    bTree,
    accumulator
  };

  g_tree_foreach(bTree->nativeTree, nativeBTreeReduce, &ctxt);

  return (napi_value) ctxt.data;
}

static napi_value esFilter(napi_value env, napi_callback_info cbInfo) {
  napi_value esThis, array, callback, accumulator, cbThis, argv[2];
  BTree_t *bTree;
  size_t argc = 2;

  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // Extract native BTree pointer
  EXTRACT_BTREE(env, esThis, bTree);

  CHECK_ARGC(1, msgTooFewArguments);
  callback = argv[0];

  if (argc > 1) {
    cbThis = argv[1];
  }
  else {
    NAPI_CALL(env, true,
      napi_get_global(env, &cbThis));
  }

  napi_value esbTreeConstructor, comparatorFunc;
  NAPI_CALL(env, true,
    napi_get_reference_value(env, constructor, &esbTreeConstructor));

  NAPI_CALL(env, true,
    napi_get_reference_value(env, bTree->comparator, &comparatorFunc));

  NAPI_CALL(env, true,
    napi_new_instance(env, esbTreeConstructor, 1, &comparatorFunc, &accumulator));

  ForEachContext_t ctxt = {
    esThis,
    callback,
    cbThis,
    0,
    bTree,
    accumulator
  };

  g_tree_foreach(bTree->nativeTree, nativeBTreeFilter, &ctxt);

  return (napi_value) ctxt.data;
}

/**
 * ES callback. Constructor
 */
static napi_value esConstructor(napi_env env, napi_callback_info cbInfo) {
  napi_value esBtree, comparator, isConstructor;
  napi_ref ref;

  size_t argc = 1;
  napi_value argv[1];

  // Get es arguments & context
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esBtree, NULL));

  CHECK_ARGC(1, msgTooFewArguments);
  comparator = argv[0];

  NAPI_CALL(env, true,
    napi_get_new_target(env, cbInfo, &isConstructor));

  if (isConstructor) {
    // Check type of first argument. Must be function
    napi_valuetype comparatorType;
    NAPI_CALL(env, false,
      napi_typeof(env, comparator, &comparatorType));

    if (comparatorType != napi_function) {
      NAPI_CALL(env, false,
        napi_throw_error(env, NULL, "First arg must be comparator qsort() like function"));

        return NULL;
    }

    // Allocate memory for usre data wich recived in native comparator
    BTree_t *bTree = g_new(BTree_t, 1);

    // Initialize native BTree with native comparator & additional user data
    // Key & Value is same pointer (ES object). Need free one of key or value.
    GTree *nativeTree = g_tree_new_full(nativeComparator, bTree, NULL, freeTreeValue);

    // Fill user data
    bTree->nativeTree = nativeTree;
    bTree->env = env;

    // Create ref on comparator function. Protect from GC
    NAPI_CALL(env, false,
      napi_create_reference(env, argv[0], 1, &bTree->comparator));

    // Wrap native data in ES variable for native access again
    NAPI_CALL(env, false,
      napi_wrap(env, esBtree, bTree, freeNativeBTree, NULL, &ref));
  }
  else {
    napi_value constructorFunc;

    NAPI_CALL(env, true,
      napi_get_reference_value(env, constructor, &constructorFunc));

    NAPI_CALL(env, true,
      napi_new_instance(env, constructorFunc, argc, argv, &esBtree));
  }

  return esBtree;
}

static void fromArrayCallback(ForEachContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value key, value, box;
  bool valueIsArray = (bool) ctxt->data;

  if (valueIsArray) {
    NAPI_CALL(env, true,
      napi_get_element(env, ctxt->cbThis, 0, &key));

    NAPI_CALL(env, true,
      napi_get_element(env, ctxt->cbThis, 1, &value));
  }
  // Is Object
  else {
    NAPI_CALL(env, true,
      napi_get_named_property(env, ctxt->cbThis, KEY, &key));

    NAPI_CALL(env, true,
      napi_get_named_property(env, ctxt->cbThis, VALUE, &value));
  }

  NAPI_CALL(env, true,
    napi_create_object(env, &box));

  NAPI_CALL(env, true,
    napi_set_named_property(env, box, KEY, key));

  NAPI_CALL(env, true,
    napi_set_named_property(env, box, VALUE, value));

  nativeInsertNode(env, ctxt->esbTree, box);
}

static void iterate(napi_env env, napi_value iterable, iteratorResultCallback callback, ForEachContext_t *ctxt) {
  napi_value generator, SymbolIterator, iterator, next;

  NAPI_GLOBAL_SYM(env, "iterator", SymbolIterator);

  NAPI_CALL(env, true,
    napi_get_property(env, iterable,SymbolIterator, &generator));

  NAPI_CALL(env, true,
    napi_call_function(env, iterable, generator, 0, NULL, &iterator));

  NAPI_CALL(env, true,
    napi_get_named_property(env, iterator, "next", &next));

  bool isDone = true;
  bool valueIsArray = false;
  napi_value result, value, done;

  do {
    NAPI_CALL(env, true,
      napi_call_function(env, iterator, next, 0, NULL, &result));

    NAPI_CALL(env, true,
      napi_get_named_property(env, result, "done", &done));

    NAPI_CALL(env, true,
      napi_get_named_property(env, result, VALUE, &value));

    NAPI_CALL(env, true,
      napi_get_value_bool(env, done, &isDone));

    NAPI_CALL(env, true,
      napi_is_array(env, value, &valueIsArray));

    if (!isDone) {
      ctxt->cbThis = value;
      ctxt->data = (void *) valueIsArray;

      callback(ctxt);

      ctxt->idx++;
    }

  } while(!isDone);
}

static void fromKeyValueIterable(napi_env env, napi_value iterable, napi_value esbTree) {
  BTree_t *bTree;

  // Extract native BTree pointer
  EXTRACT_BTREE(env, esbTree, bTree);

  ForEachContext_t ctxt = {
    esbTree,
    NULL,
    NULL,
    0,
    bTree,
    NULL
  };

  iterate(env, iterable, fromArrayCallback, &ctxt);
}

/**
 * static ES callback. BTree.from()
 */
static napi_value esStaticFrom(napi_env env, napi_callback_info cbInfo) {
  napi_value result, BTreeConstructor, argv[2];
  napi_value global, Map;
  napi_valuetype iterableType;
  size_t argc = 2;

  // Get es arguments & context
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &BTreeConstructor, NULL));

  CHECK_ARGC(2, msgTooFewArguments);
  napi_value comparator = argv[0];
  napi_value iterable = argv[1];

  NAPI_CALL(env, true,
    napi_get_global(env, &global));
  NAPI_CALL(env, true,
    napi_get_named_property(env, global, "Map", &Map));

  NAPI_CALL(env, true,
    napi_new_instance(env, BTreeConstructor, 1, &comparator, &result));


  bool isArray = false;
  bool isMap = false;
  bool isGenericIterable = false;

  NAPI_CALL(env, true,
    napi_is_array(env, iterable, &isArray));

  NAPI_CALL(env, true,
    napi_instanceof(env, iterable, Map, &isMap));

  // May be generic?
  if (!(isArray || isMap)) {
    napi_handle_scope scope;

    NAPI_CALL(env, true,
      napi_open_handle_scope(env, &scope));

    // Duck typing. next() method exists?
    napi_value nextMethod;
    napi_valuetype nextMethodType;

    NAPI_CALL(env, true,
      napi_get_named_property(env, iterable, "next", &nextMethod));

    NAPI_CALL(env, true,
      napi_typeof(env, nextMethod, &nextMethodType));

    isGenericIterable = (nextMethodType == napi_function);

    NAPI_CALL(env, true,
      napi_close_handle_scope(env, scope));
  }

  if (isMap || isArray || isGenericIterable) {
    fromKeyValueIterable(env, iterable, result);
  }
  else {
    NAPI_CALL(env, true,
      napi_throw_error(env, NULL, "Second arg must be Array, Map or iterable"));
  }

  return result;
}

/**
 * Module initialization callback
 */
static napi_value init(napi_env env, napi_value exports) {
  napi_value esBTreeClass, symbolIterator;

  NAPI_GLOBAL_SYM(env, "iterator", symbolIterator);

  // Instance props
  napi_property_descriptor esBTreeProps[] = {
    {
      "height",
      NULL,

      NULL,
      esHeight,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "size",
      NULL,

      NULL,
      esSize,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "set",
      NULL,

      esSet,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "get",
      NULL,

      esGet,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "delete",
      NULL,

      esDelete,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      NULL,
      // [Symbol.iterator]
      symbolIterator,

      esGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      (void *) iteratorResultDefaultCb
    },
    {
      "entries",
      NULL,

      esGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      (void *) iteratorResultDefaultCb
    },
    {
      "values",
      NULL,

      esGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      (void *) iteratorResultValueCb
    },
    {
      "keys",
      NULL,

      esGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      (void *) iteratorResultKeyCb
    },
    {
      "forEach",
      NULL,

      esForeach,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "clear",
      NULL,

      esClear,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "has",
      NULL,

      esHas,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    // Extra methods
    {
      "map",
      NULL,

      esMap,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "reduce",
      NULL,

      esReduce,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "filter",
      NULL,

      esFilter,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    }
  };

  NAPI_CALL(env, false,
    napi_define_class(env, "BTree", NAPI_AUTO_LENGTH, esConstructor, NULL, (sizeof(esBTreeProps) / sizeof(esBTreeProps[0])), esBTreeProps, &esBTreeClass));

  NAPI_CALL(env, false,
    napi_create_reference(env, esBTreeClass, 1, &constructor));

  // Static props
  napi_property_descriptor staticProps[] = {{
    "from",
    NULL,

    esStaticFrom,
    NULL,
    NULL,
    NULL,

    napi_default,
    NULL
  }};

  NAPI_CALL(env, true,
    napi_define_properties(env, esBTreeClass, (sizeof(staticProps) / sizeof(staticProps)), staticProps));

  // export
  napi_property_descriptor props[] = {{
    "BTree",
    NULL,
    NULL,
    NULL,
    NULL,
    esBTreeClass,
    napi_default,
    NULL
  }};

  NAPI_CALL(env, false,
    napi_define_properties(env, exports, (sizeof(props) / sizeof(props[0])), props));

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
