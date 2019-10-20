#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <node_api.h>
#include <glib.h>

#include "include/common.h"
#include "include/glib_local.h"

typedef enum ITERATOR_STATE {
  ITERATOR_INIT,
  ITERATOR_LOOP,
  ITERATOR_END
} IteratorState_t;

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
  napi_value esCallback;
} BTreeTraverseData_t;

typedef struct {
  BTree_t *bTree;
  IteratorState_t state;
  GTreeNode_t currentNode;
  napi_value value;
} BTreeIteratorContext_t;

typedef void (* iteratorResultCallback) (BTreeIteratorContext_t *ctxt);

static napi_ref constructor;

static inline void iteratorResultDefaultCb(BTreeIteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_ref esValueRef;
  napi_value esValue, tmp;

  esValueRef = (napi_ref) local_g_tree_node_value(ctxt->currentNode);
  NAPI_CALL(env, napi_get_reference_value(env, esValueRef, &esValue));

  NAPI_CALL(env, napi_create_array_with_length(env, 2, &ctxt->value));

  NAPI_CALL(env, napi_get_named_property(env, esValue, "key", &tmp));
  NAPI_CALL(env, napi_set_element(env, ctxt->value, 0, tmp));

  NAPI_CALL(env, napi_get_named_property(env, esValue, "value", &tmp));
  NAPI_CALL(env, napi_set_element(env, ctxt->value, 1, tmp));
}

static inline void iteratorResultKeyCb(BTreeIteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_ref esValueRef;
  napi_value esValue;

  esValueRef = (napi_ref) local_g_tree_node_value(ctxt->currentNode);
  NAPI_CALL(env, napi_get_reference_value(env, esValueRef, &esValue));

  NAPI_CALL(env, napi_get_named_property(env, esValue, "key", &ctxt->value));
}

static inline void iteratorResultValueCb(BTreeIteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_ref esValueRef;
  napi_value esValue;

  esValueRef = (napi_ref) local_g_tree_node_value(ctxt->currentNode);
  NAPI_CALL(env, napi_get_reference_value(env, esValueRef, &esValue));

  NAPI_CALL(env, napi_get_named_property(env, esValue, "value", &ctxt->value));
}

static inline gboolean _removeTreeNode(gpointer key, gpointer val, gpointer data) {
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
}

/**
 * Free allocated GTree & unref comparator for GC access
 */
void freeNativeBTree(napi_env env, void *finalize_data, void *finalize_hint) {
  BTree_t *bTree = (BTree_t *) finalize_data;

  // Remove all nodes
  g_tree_foreach(bTree->nativeTree, (gpointer) freeNativeBTreeNode, (gpointer) bTree);

  // Destroy native bTree
  g_tree_destroy(bTree->nativeTree);

  // Release BTree_t struct memory
  g_free((gpointer) bTree);
}

void freeIterator(napi_env env, void *finalize_data, void *finalize_hint) {
  printf("Free iterator.\n");
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
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &bTree));

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
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &bTree));

  // Native call to glib tree
  gint nativeSize = g_tree_nnodes(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env, napi_create_int64(env, (int64_t) nativeSize, &esSize));

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
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &bTree));

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
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &bTree));

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
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &bTree));

  BTreeTraverseData_t traverseData = {
    bTree,
    NULL
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
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &bTree));

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
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &bTree));

  napi_value key = argv[0];
  napi_value lookupBox;
  napi_ref lookupRef;

  NAPI_CALL(env, napi_create_object(env, &lookupBox));
  NAPI_CALL(env, napi_set_named_property(env, lookupBox, "key", key));
  NAPI_CALL(env, napi_create_reference(env, lookupBox, 0, &lookupRef));

  // Native call to glib tree
  napi_ref lookupResult = (napi_ref) g_tree_lookup(bTree->nativeTree, lookupRef);

  if (lookupResult == NULL) {
    NAPI_CALL(env, napi_get_undefined(env, &result));
  }
  else {
    NAPI_CALL(env, napi_get_reference_value(env, lookupResult, &result));
    NAPI_CALL(env, napi_get_named_property(env, result, "value", &result));
  }

  return result;
}

napi_value esIteratorNext(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTreeIteratorContext_t *itCtxt;
  iteratorResultCallback resultCb;

  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, (void **) &resultCb));

  // Extract native pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &itCtxt));

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
  NAPI_CALL(env, napi_create_object(env, &esIteratorResult));

  if (itCtxt->state == ITERATOR_END) {
    NAPI_CALL(env, napi_get_undefined(env, &itCtxt->value));
    NAPI_CALL(env, napi_get_boolean(env, true, &isDone));
  }
  else {
    resultCb(itCtxt);
    NAPI_CALL(env, napi_get_boolean(env, false, &isDone));
  }

  NAPI_CALL(env, napi_set_named_property(env, esIteratorResult, "value", itCtxt->value));
  NAPI_CALL(env, napi_set_named_property(env, esIteratorResult, "done", isDone));

  return esIteratorResult;
}

napi_value esBTreeGenerator(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, esIterator;
  BTree_t *bTree;
  void *data;

  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, &data));

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &bTree));

  // Create es Iterator
  NAPI_CALL(env, napi_create_object(env, &esIterator));

  napi_value nextFunction;
  NAPI_CALL(env,
    napi_create_function(env, "next", NAPI_AUTO_LENGTH, esIteratorNext, data, &nextFunction));

  NAPI_CALL(env,
    napi_set_named_property(env, esIterator, "next", nextFunction));

  BTreeIteratorContext_t *itCtxt = (BTreeIteratorContext_t *) malloc(sizeof(BTreeIteratorContext_t));
  itCtxt->bTree = bTree;
  itCtxt->state = ITERATOR_INIT;

  napi_ref ref;
  NAPI_CALL(env, napi_wrap(env, esIterator, (void *) itCtxt, freeIterator, NULL, &ref));

  return esIterator;
}

gboolean nativeBTreeTraverse(gpointer key, gpointer val, gpointer data) {
  napi_ref objectRef = (napi_ref) val;
  napi_value esObject, esKey, esValue, esNull;
  BTreeTraverseData_t *tData = (BTreeTraverseData_t *) data;
  napi_env env = tData->bTree->env;

  NAPI_CALL(env, napi_get_reference_value(env, objectRef, &esObject));
  NAPI_CALL(env, napi_get_named_property(env, esObject, "key", &esKey));
  NAPI_CALL(env, napi_get_named_property(env, esObject, "value", &esValue));

  napi_value argv[] = { esKey, esValue };
  NAPI_CALL(env, napi_get_null(env, &esNull));

  NAPI_CALL(env, napi_call_function(env, esNull, tData->esCallback, 2, argv, NULL));

  return FALSE;
}

napi_value esBTreeForeach(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, undefined;
  BTree_t *bTree;
  size_t argc = 1;
  napi_value argv[1];


  // Get es this for current btree
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env, napi_throw_error(env, NULL, "Too few arguments."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, (void **) &bTree));

  BTreeTraverseData_t traverseData = {
    bTree,
    argv[0] // es Callback
  };

  g_tree_foreach(bTree->nativeTree, nativeBTreeTraverse, &traverseData);

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
  NAPI_CALL(env, napi_create_reference(env, argv[0], 0, &bTree->comparator));

  // Wrap native data in ES variable for native access again
  NAPI_CALL(env, napi_wrap(env, esBtree, bTree, freeNativeBTree, NULL, &ref)); // NOTE: Pass

  return esBtree;
}

napi_value init(napi_env env, napi_value exports) {
  napi_value esBTreeClass, symbolIterator;

  NAPI_GLOBAL_SYM(env, "iterator", symbolIterator);

  napi_property_descriptor esBTreeProps[] = {
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
      (void *) iteratorResultDefaultCb
    },
    {
      "entries",
      NULL,

      esBTreeGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      (void *) iteratorResultDefaultCb
    },
    {
      "values",
      NULL,

      esBTreeGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      (void *) iteratorResultValueCb
    },
    {
      "keys",
      NULL,

      esBTreeGenerator,
      NULL,
      NULL,
      NULL,

      napi_default,
      (void *) iteratorResultKeyCb
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

  NAPI_CALL(env,
    napi_define_class(env, "BTree", NAPI_AUTO_LENGTH, esBTreeConstructor, NULL, (sizeof(esBTreeProps) / sizeof(esBTreeProps[0])), esBTreeProps, &esBTreeClass));

  NAPI_CALL(env,
    napi_create_reference(env, esBTreeClass, 1, &constructor)); // NOTE: Pass

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
