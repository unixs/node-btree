#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <node_api.h>
#include <glib.h>

#include "include/common.h"
#include "include/glib_local.h"

enum state {
  TRAVERSE_INIT,
  TRAVERSE_LOOP,
  TRAVERSE_END
};

typedef struct {
  napi_env env;
  napi_ref comparator;
  GTree *nativeTree;
} BTree_t;

typedef struct {
  napi_env env;
  napi_ref ref;
} TreeNode_t;

typedef struct {
  BTree_t *bTree;
  napi_value esCb;
} BTreeTraverseData_t;

typedef struct {
  BTree_t *bTree;
  enum state state;
  GTreeNode *currentNode;
  napi_value value;
} BTreeTraverseContext_t;

static napi_ref constructor;

gboolean _removeTreeNode(gpointer key, gpointer val, gpointer data) {
  napi_ref keyRef = (napi_ref) key;
  BTreeTraverseData_t *tData = (BTreeTraverseData_t *) data;

  g_tree_remove(tData->bTree->nativeTree, keyRef);

  return FALSE;
}

/**
 * Unref all bTree nodes for GC access
 */
static void freeNativeBTreeNode(gpointer key, gpointer value, gpointer data) {
  napi_ref objRef = (napi_ref) key;
  BTree_t *bTree = (BTree_t *) data;

  NAPI_CALL(bTree->env, napi_delete_reference(bTree->env, objRef));

  return FALSE;
}

/**
 * Free allocated GTree & unref comparator for GC access
 */
void freeNativeBTree(napi_env env, void *finalize_data, void *finalize_hint) {
  BTree_t *bTree = (BTree_t *) finalize_data;

  // Destroy ref to comparator function for GC access
  NAPI_CALL(env, napi_delete_reference(env, bTree->comparator));

  // Remove all nodes
  g_tree_foreach(bTree->nativeTree, freeNativeBTreeNode, (gpointer) bTree);

  // Destroy native bTree
  g_tree_destroy(bTree->nativeTree);

  // Release native bTree memory
  g_free((gpointer) bTree->nativeTree);

  // Release BTree_t struct memory
  g_free((gpointer) bTree);
}

static inline gint nativeComparator(gconstpointer a, gconstpointer b, gpointer bTree) {
  int64_t compareResult = 0;

  BTree_t *bTreeWrap = (BTree_t *) bTree;
  napi_env env = bTreeWrap->env;

  napi_ref refA = (napi_ref) a;
  napi_ref refB = (napi_ref) b;
  napi_value boxA, boxB, keyA, keyB, esNull, esResult, comparator;

  NAPI_CALL(env, napi_get_reference_value(env, refA, &boxA));
  NAPI_CALL(env, napi_get_reference_value(env, refB, &boxB));
  NAPI_CALL(env, napi_get_named_property(env, boxA, "key", &keyA));
  NAPI_CALL(env, napi_get_named_property(env, boxB, "key", &keyB));
  NAPI_CALL(env, napi_get_null(env, &esNull));

  NAPI_CALL(env, napi_get_reference_value(env, bTreeWrap->comparator, &comparator));
  napi_value argv[] = {keyA, keyB};

  NAPI_CALL(env, napi_call_function(env, esNull, comparator, 2, argv, &esResult));

  NAPI_CALL(env, napi_get_value_int64(env, esResult, &compareResult));

  return (gint) compareResult;
}

napi_value esBTreeHeight(napi_env env, napi_callback_info cbInfo) {
  napi_value esHeight;
  napi_value esThis;
  BTree_t *bTree;

  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  // Native call to glib tree
  gint nativeHeight = g_tree_height(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env, napi_create_int64(env, nativeHeight, &esHeight));

  return esHeight;
}

napi_value esBTreeSize(napi_env env, napi_callback_info cbInfo) {
  napi_value esSize;
  napi_value esThis;
  BTree_t *bTree;

  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  // Native call to glib tree
  gint nativeSize = g_tree_nnodes(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env, napi_create_int64(env, nativeSize, &esSize));

  return esSize;
}

napi_value esBTreeDelete(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, result, box;
  napi_ref boxRef;
  BTree_t *bTree;

  size_t argc = 1;
  napi_value argv[1];

  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env, napi_throw_error(env, NULL, "Too few arguments."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  NAPI_CALL(env, napi_create_object(env, &box));
  NAPI_CALL(env, napi_set_named_property(env, box, "key", argv[0]));
  NAPI_CALL(env, napi_create_reference(env, box, 0, &boxRef));

  gboolean found = FALSE;

  // Find value
  napi_ref esObjRef = g_tree_lookup(bTree->nativeTree, boxRef);

  if (esObjRef != NULL) {
    found = TRUE;

    // Remove bTree node
    g_tree_remove(bTree->nativeTree, boxRef);

    // Unref es value for GC access
    NAPI_CALL(env, napi_delete_reference(env, boxRef));
  }

  NAPI_CALL(env, napi_get_boolean(env, found, &result));

  return result;
}

napi_value esBTreeSet(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTree_t *bTree;
  size_t argc = 2;
  napi_value argv[2], key, value, box;

  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 2) {
    NAPI_CALL(env, napi_throw_error(env, NULL, "Expected two arguments."));
  }

  key = argv[0];
  value = argv[1];

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  NAPI_CALL(env, napi_create_object(env, &box));
  NAPI_CALL(env, napi_set_named_property(env, box, "key", key));
  NAPI_CALL(env, napi_set_named_property(env, box, "value", value));

  napi_ref boxRef;

  // Attach env to es object for correct unref on destroy
  // NAPI_CALL(env, napi_wrap(env, box, env, NULL, NULL, &boxRef));

  // size_t refCnt;
  // NAPI_CALL(env, napi_reference_ref(env, boxRef, NULL));
  NAPI_CALL(env, napi_create_reference(env, box, 1, &boxRef)); // NOTE: Pass

  // Native call to glib tree
  bTree->env = env;

  g_tree_insert(bTree->nativeTree, boxRef, boxRef);

  return esThis;
}

napi_value esClear(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTree_t *bTree;

  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  BTreeTraverseData_t traverseData = {
    bTree
  };

  g_tree_foreach(bTree->nativeTree, _removeTreeNode, &traverseData);

  napi_value result;
  NAPI_CALL(env, napi_get_undefined(env, &result));

  return result;
}

napi_value esHas(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  napi_ref boxRef;
  BTree_t *bTree;
  size_t argc = 1;
  napi_value argv[1];

  napi_value box;

  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env, napi_throw_error(env, NULL, "Too few arguments."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  NAPI_CALL(env, napi_create_object(env, &box));
  NAPI_CALL(env, napi_set_named_property(env, box, "key", argv[0]));
  NAPI_CALL(env, napi_create_reference(env, box, 0, &boxRef));

  gpointer found = g_tree_lookup(bTree->nativeTree, boxRef);

  napi_value result;

  if (found == NULL) {
    NAPI_CALL(env, napi_get_boolean(env, false, &result));
  }
  else {
    NAPI_CALL(env, napi_get_boolean(env, true, &result));
  }

  return result;
}

napi_value esBTreeGet(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  napi_value result;
  BTree_t *bTree;
  size_t argc = 1;
  napi_value argv[1];

  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env, napi_throw_error(env, "10", "Expected one argument."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  napi_value key = argv[0];
  napi_value lookupBox;
  napi_ref lookupRef;

  NAPI_CALL(env, napi_create_object(env, &lookupBox));
  NAPI_CALL(env, napi_set_named_property(env, lookupBox, "key", key));
  NAPI_CALL(env, napi_create_reference(env, lookupBox, 0, &lookupRef));

  // Native call to glib tree
  napi_ref lookupResult = g_tree_lookup(bTree->nativeTree, lookupRef);

  if (lookupResult == NULL) {
    NAPI_CALL(env, napi_get_undefined(env, &result));
  }
  else {
    NAPI_CALL(env, napi_get_reference_value(env, lookupResult, &result));
    NAPI_CALL(env, napi_get_named_property(env, result, "value", &result));
  }

  return result;
}

void freeIterator(napi_env env, void *finalize_data, void *finalize_hint) {
  printf("Free iterator.\n");
}

napi_value esBTreeIteratorNext(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTreeTraverseContext_t *traverseContext;

  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &traverseContext));

  napi_ref esValueRef;
  napi_value box;
  napi_value array[2];
  GTreeNode *node;

  switch (traverseContext->state) {
    case TRAVERSE_INIT:
      traverseContext->state = TRAVERSE_LOOP;
      node = local_g_tree_first_node(traverseContext->bTree->nativeTree);
      traverseContext->currentNode = node;

      esValueRef = (napi_ref) node->value;
      NAPI_CALL(env, napi_get_reference_value(env, esValueRef, &box)); // es: { key, value }
      NAPI_CALL(env, napi_create_array_with_length(env, 2, &traverseContext->value));
      NAPI_CALL(env, napi_get_named_property(env, box, "key", &array[0]));
      NAPI_CALL(env, napi_set_element(env, traverseContext->value, 0, array[0]));
      NAPI_CALL(env, napi_get_named_property(env, box, "value", &array[1]));
      NAPI_CALL(env, napi_set_element(env, traverseContext->value, 1, array[1]));
      break;

    case TRAVERSE_LOOP:
      node = local_g_tree_node_next(traverseContext->currentNode);

      traverseContext->currentNode = node;

      if (node == NULL) {
        traverseContext->state = TRAVERSE_END;
        break;
      }

      esValueRef = (napi_ref) node->value;
      NAPI_CALL(env, napi_get_reference_value(env, esValueRef, &box)); // es: { key, value }
      NAPI_CALL(env, napi_create_array_with_length(env, 2, &traverseContext->value));
      NAPI_CALL(env, napi_get_named_property(env, box, "key", &array[0]));
      NAPI_CALL(env, napi_set_element(env, traverseContext->value, 0, array[0]));
      NAPI_CALL(env, napi_get_named_property(env, box, "value", &array[1]));
      NAPI_CALL(env, napi_set_element(env, traverseContext->value, 1, array[1]));
      break;

    case TRAVERSE_END:
      // noop
      break;
  }

  napi_value esIteratorResult, isDone;
  NAPI_CALL(env, napi_create_object(env, &esIteratorResult));

  if (traverseContext->state == TRAVERSE_END) {
    NAPI_CALL(env, napi_get_undefined(env, &traverseContext->value));
    NAPI_CALL(env, napi_get_boolean(env, true, &isDone));
  }
  else {
    NAPI_CALL(env, napi_get_boolean(env, false, &isDone));
  }

  NAPI_CALL(env, napi_set_named_property(env, esIteratorResult, "value", traverseContext->value));
  NAPI_CALL(env, napi_set_named_property(env, esIteratorResult, "done", isDone));


  return esIteratorResult;
}

napi_value esBTreeIteratorKeysNext(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTreeTraverseContext_t *traverseContext;

  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &traverseContext));

  napi_ref esValueRef;
  napi_value box;
  GTreeNode *node;

  switch (traverseContext->state) {
    case TRAVERSE_INIT:
      traverseContext->state = TRAVERSE_LOOP;
      node = local_g_tree_first_node(traverseContext->bTree->nativeTree);
      traverseContext->currentNode = node;

      esValueRef = (napi_ref) node->value;
      NAPI_CALL(env, napi_get_reference_value(env, esValueRef, &box)); // es: { key, value }
      NAPI_CALL(env, napi_get_named_property(env, box, "key", &traverseContext->value));
      break;

    case TRAVERSE_LOOP:
      node = local_g_tree_node_next(traverseContext->currentNode);

      traverseContext->currentNode = node;

      if (node == NULL) {
        traverseContext->state = TRAVERSE_END;
        break;
      }

      esValueRef = (napi_ref) node->value;
      NAPI_CALL(env, napi_get_reference_value(env, esValueRef, &box)); // es: { key, value }
      NAPI_CALL(env, napi_get_named_property(env, box, "key", &traverseContext->value));
      break;

    case TRAVERSE_END:
      // noop
      break;
  }

  napi_value esIteratorResult, isDone;
  NAPI_CALL(env, napi_create_object(env, &esIteratorResult));

  if (traverseContext->state == TRAVERSE_END) {
    NAPI_CALL(env, napi_get_undefined(env, &traverseContext->value));
    NAPI_CALL(env, napi_get_boolean(env, true, &isDone));
  }
  else {
    NAPI_CALL(env, napi_get_boolean(env, false, &isDone));
  }

  NAPI_CALL(env, napi_set_named_property(env, esIteratorResult, "value", traverseContext->value));
  NAPI_CALL(env, napi_set_named_property(env, esIteratorResult, "done", isDone));


  return esIteratorResult;
}

napi_value esBTreeIteratorValuesNext(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTreeTraverseContext_t *traverseContext;

  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &traverseContext));

  napi_ref esValueRef;
  napi_value box;
  GTreeNode *node;

  switch (traverseContext->state) {
    case TRAVERSE_INIT:
      traverseContext->state = TRAVERSE_LOOP;
      node = local_g_tree_first_node(traverseContext->bTree->nativeTree);
      traverseContext->currentNode = node;

      esValueRef = (napi_ref) node->value;
      NAPI_CALL(env, napi_get_reference_value(env, esValueRef, &box)); // es: { key, value }
      NAPI_CALL(env, napi_get_named_property(env, box, "value", &traverseContext->value));
      break;

    case TRAVERSE_LOOP:
      node = local_g_tree_node_next(traverseContext->currentNode);

      traverseContext->currentNode = node;

      if (node == NULL) {
        traverseContext->state = TRAVERSE_END;
        break;
      }

      esValueRef = (napi_ref) node->value;
      NAPI_CALL(env, napi_get_reference_value(env, esValueRef, &box)); // es: { key, value }
      NAPI_CALL(env, napi_get_named_property(env, box, "value", &traverseContext->value));
      break;

    case TRAVERSE_END:
      // noop
      break;
  }

  napi_value esIteratorResult, isDone;
  NAPI_CALL(env, napi_create_object(env, &esIteratorResult));

  if (traverseContext->state == TRAVERSE_END) {
    NAPI_CALL(env, napi_get_undefined(env, &traverseContext->value));
    NAPI_CALL(env, napi_get_boolean(env, true, &isDone));
  }
  else {
    NAPI_CALL(env, napi_get_boolean(env, false, &isDone));
  }

  NAPI_CALL(env, napi_set_named_property(env, esIteratorResult, "value", traverseContext->value));
  NAPI_CALL(env, napi_set_named_property(env, esIteratorResult, "done", isDone));


  return esIteratorResult;
}

napi_value esBTreeGenerator(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, esIterator;
  BTree_t *bTree;

  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  // Create es Iterator
  NAPI_CALL(env, napi_create_object(env, &esIterator));

  napi_value nextFunction;
  NAPI_CALL(env, napi_create_function(env, "next", NAPI_AUTO_LENGTH, esBTreeIteratorNext, NULL, &nextFunction));
  NAPI_CALL(env, napi_set_named_property(env, esIterator, "next", nextFunction));

  BTreeTraverseContext_t *traverseContext = (BTreeTraverseContext_t *) malloc(sizeof(BTreeTraverseContext_t));
  traverseContext->bTree = bTree;
  traverseContext->state = TRAVERSE_INIT;

  napi_value ref;
  NAPI_CALL(env, napi_wrap(env, esIterator, traverseContext, freeIterator, NULL, &ref));

  return esIterator;
}

napi_value esBTreeKeysGenerator(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, esIterator;
  BTree_t *bTree;

  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  // Create es Iterator
  NAPI_CALL(env, napi_create_object(env, &esIterator));

  napi_value nextFunction;
  NAPI_CALL(env, napi_create_function(env, "next", NAPI_AUTO_LENGTH, esBTreeIteratorKeysNext, NULL, &nextFunction));
  NAPI_CALL(env, napi_set_named_property(env, esIterator, "next", nextFunction));

  BTreeTraverseContext_t *traverseContext = (BTreeTraverseContext_t *) malloc(sizeof(BTreeTraverseContext_t));
  traverseContext->bTree = bTree;
  traverseContext->state = TRAVERSE_INIT;

  napi_value ref;
  NAPI_CALL(env, napi_wrap(env, esIterator, traverseContext, freeIterator, NULL, &ref));

  return esIterator;
}

napi_value esBTreeValuesGenerator(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, esIterator;
  BTree_t *bTree;

  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  // Create es Iterator
  NAPI_CALL(env, napi_create_object(env, &esIterator));

  napi_value nextFunction;
  NAPI_CALL(env, napi_create_function(env, "next", NAPI_AUTO_LENGTH, esBTreeIteratorValuesNext, NULL, &nextFunction));
  NAPI_CALL(env, napi_set_named_property(env, esIterator, "next", nextFunction));

  BTreeTraverseContext_t *traverseContext = (BTreeTraverseContext_t *) malloc(sizeof(BTreeTraverseContext_t));
  traverseContext->bTree = bTree;
  traverseContext->state = TRAVERSE_INIT;

  napi_value ref;
  NAPI_CALL(env, napi_wrap(env, esIterator, traverseContext, freeIterator, NULL, &ref));

  return esIterator;
}

gboolean BTreeNativeTraverse(gpointer key, gpointer val, gpointer data) {
  napi_ref objectRef = (napi_ref) val;
  napi_value esObject, esKey, esValue, esNull;
  BTreeTraverseData_t *tData = (BTreeTraverseData_t *) data;
  napi_env env = tData->bTree->env;

  NAPI_CALL(env, napi_get_reference_value(env, objectRef, &esObject));
  NAPI_CALL(env, napi_get_named_property(env, esObject, "key", &esKey));
  NAPI_CALL(env, napi_get_named_property(env, esObject, "value", &esValue));

  napi_value argv[] = { esKey, esValue };
  NAPI_CALL(env, napi_get_null(env, &esNull));

  NAPI_CALL(env, napi_call_function(env, esNull, tData->esCb, 2, argv, NULL));

  return FALSE;
}

napi_value esBTreeForeach(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, undefined;
  BTree_t *bTree;
  size_t argc = 1;
  napi_value argv[1], esCb;


  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env, napi_throw_error(env, NULL, "Too few arguments."));
  }

  esCb = argv[0];
  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  BTreeTraverseData_t traverseData = {
    bTree,
    esCb
  };

  g_tree_foreach(bTree->nativeTree, BTreeNativeTraverse, &traverseData);

  NAPI_CALL(env, napi_get_undefined(env, &undefined));

  return undefined;
}

static napi_value esBTreeConstructor(napi_env env, napi_callback_info cbInfo) {
  napi_value esBtree;
  napi_ref ref;

  size_t argc = 1;
  napi_value argv[1];
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, argv, &esBtree, NULL));

  // Allocate memory for usre data wich recived in native comparator
  BTree_t *bTree = (BTree_t *) malloc(sizeof(BTree_t)); // NOTE: Pass

  // Initialize native BTree with native comparator & additional user data
  GTree *nativeTree = g_tree_new_with_data(nativeComparator, bTree);

  napi_valuetype comparatorType;
  NAPI_CALL(env, napi_typeof(env, argv[0], &comparatorType));

  if (comparatorType != napi_function) {
    NAPI_CALL(env, napi_throw_error(env, "10", "First arg must be comparator function"));
  }

  // Fill user data
  bTree->nativeTree = nativeTree;
  bTree->env = env;
  NAPI_CALL(env, napi_create_reference(env, argv[0], 1, &bTree->comparator)); // NOTE: Pass

  // Wrap native data in ES variable for native access again
  NAPI_CALL(env, napi_wrap(env, esBtree, bTree, freeNativeBTree, NULL, &ref)); // NOTE: Pass

  // TODO: Need macros for get symbol
  napi_value global, Symbol, symbolIterator;
  NAPI_CALL(env, napi_get_global(env, &global));
  NAPI_CALL(env, napi_get_named_property(env, global, "Symbol", &Symbol));
  NAPI_CALL(env, napi_get_named_property(env, Symbol, "iterator", &symbolIterator));

  // Define comparator as not enumerable & ro property of es btree instance
  napi_property_descriptor esBTreeProps[] = {
    {
      "comparator",
      NULL,

      NULL,
      NULL,
      NULL,
      bTree->comparator,

      napi_default,
      NULL
    },
    {
      "height",
      NULL,

      NULL,
      esBTreeHeight,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "size",
      NULL,

      NULL,
      esBTreeSize,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "set",
      NULL,

      esBTreeSet,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "get",
      NULL,

      esBTreeGet,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "delete",
      NULL,

      esBTreeDelete,
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

      esBTreeGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "entries",
      NULL,

      esBTreeGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "values",
      NULL,

      esBTreeValuesGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "keys",
      NULL,

      esBTreeKeysGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      NULL
    },
    {
      "forEach",
      NULL,

      esBTreeForeach,
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
    }
  };

  NAPI_CALL(env, napi_define_properties(env, esBtree, (sizeof(esBTreeProps) / sizeof(esBTreeProps[0])), esBTreeProps));

  return esBtree;
}

napi_value init(napi_env env, napi_value exports) {
  napi_value esBTreeClass;

  NAPI_CALL(env, napi_define_class(env, "BTree", NAPI_AUTO_LENGTH, esBTreeConstructor, NULL, 0, NULL, &esBTreeClass));
  NAPI_CALL(env, napi_create_reference(env, esBTreeClass, 1, &constructor)); // NOTE: Pass

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

  NAPI_CALL(env, napi_define_properties(env, exports, (sizeof(props) / sizeof(props[0])), props));

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
