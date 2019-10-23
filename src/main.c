#include <node_api.h>

#include "include/common.h"
#include "include/glib_local.h"

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
  napi_env env;
  napi_ref comparator;
  GTree *nativeTree;
} BTree_t;

/**
 * Context for native iterations with additional data
 */
typedef struct {
  BTree_t *bTree;
  napi_value esCallback;
} TraverseData_t;

/**
 * ES iterator native context
 */
typedef struct {
  BTree_t *bTree;
  IteratorState_t state;
  GTreeNode_t currentNode;
  napi_value value;
} IteratorContext_t;

/**
 * Native callback for generic operations
 */
typedef void (*iteratorResultCallback)(IteratorContext_t *ctxt);

static napi_ref constructor;

/**
 * Callback for iterator wich return key with value
 */
static inline void iteratorResultDefaultCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_ref esValueRef;
  napi_value esValue, tmp;

  esValueRef = (napi_ref) local_g_tree_node_value(ctxt->currentNode);
  NAPI_CALL(env,
    napi_get_reference_value(env, esValueRef, &esValue));

  NAPI_CALL(env,
    napi_create_array_with_length(env, 2, &ctxt->value));

  NAPI_CALL(env,
    napi_get_named_property(env, esValue, "key", &tmp));
  NAPI_CALL(env,
    napi_set_element(env, ctxt->value, 0, tmp));

  NAPI_CALL(env,
    napi_get_named_property(env, esValue, "value", &tmp));
  NAPI_CALL(env,
    napi_set_element(env, ctxt->value, 1, tmp));
}

/**
 * Callback for iterator wich return key only
 */
static inline void iteratorResultKeyCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_ref esValueRef;
  napi_value esValue;

  esValueRef = (napi_ref) local_g_tree_node_value(ctxt->currentNode);
  NAPI_CALL(env,
    napi_get_reference_value(env, esValueRef, &esValue));

  NAPI_CALL(env,
    napi_get_named_property(env, esValue, "key", &ctxt->value));
}

/**
 * Callback for iterator wich return value only
 */
static inline void iteratorResultValueCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_ref esValueRef;
  napi_value esValue;

  esValueRef = (napi_ref) local_g_tree_node_value(ctxt->currentNode);
  NAPI_CALL(env,
    napi_get_reference_value(env, esValueRef, &esValue));

  NAPI_CALL(env,
    napi_get_named_property(env, esValue, "value", &ctxt->value));
}

/**
 * Unref all bTree nodes for GC access
 */
static inline void unrefBTreeNode(gpointer key, gpointer value, gpointer data) {
  napi_ref objRef = (napi_ref) key;
  BTree_t *bTree = (BTree_t *) data;

  NAPI_CALL(bTree->env,
    napi_delete_reference(bTree->env, objRef));
}

/**
 * Unref all bTree nodes for GC access. Wrapper with typechecking.
 */
static inline void unrefBTreeNodeTyped(napi_ref key, BTree_t *bTree) {
  unrefBTreeNode((gpointer) key, NULL, (gpointer) bTree);
}

/**
 * Unref all bTree nodes for GC access.
 */
static inline gboolean removeTreeNode(gpointer key, gpointer val, gpointer data) {
  napi_ref keyRef = (napi_ref) key;
  BTree_t *bTree = (BTree_t *) data;

  // Remove es value from native bTree
  g_tree_remove(bTree->nativeTree, keyRef);

  // Unref es value for GC access
  unrefBTreeNodeTyped(keyRef, bTree);

  return FALSE;
}

/**
 * Unref all bTree nodes for GC access. Wrapper with typechecking.
 */
static inline gboolean removeTreeNodeTyped(napi_ref key, BTree_t *bTree) {
  return removeTreeNode((gpointer) key, NULL, (gpointer) bTree);
}

/**
 * Free allocated GTree & unref comparator for GC access
 */
static void freeNativeBTree(napi_env env, void *finalize_data, void *finalize_hint) {
  BTree_t *bTree = (BTree_t *) finalize_data;

  // Unref all nodes for GC access
  g_tree_foreach(bTree->nativeTree, (gpointer) unrefBTreeNode, (gpointer) bTree);

  // Unref comparator function for GC access
  NAPI_CALL(env,
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
 * Native comparator function
 */
static inline gint nativeComparator(gconstpointer a, gconstpointer b, gpointer bTree) {
  int64_t compareResult = 0;

  BTree_t *bTreeWrap = (BTree_t *) bTree;
  napi_env env = bTreeWrap->env;

  napi_ref refA = (napi_ref) a;
  napi_ref refB = (napi_ref) b;
  napi_value boxA, boxB, keyA, keyB, esNull, esResult, comparator;

  NAPI_CALL(env,
    napi_get_reference_value(env, refA, &boxA));

  NAPI_CALL(env,
    napi_get_reference_value(env, refB, &boxB));

  NAPI_CALL(env,
    napi_get_named_property(env, boxA, "key", &keyA));

  NAPI_CALL(env,
    napi_get_named_property(env, boxB, "key", &keyB));

  NAPI_CALL(env,
    napi_get_null(env, &esNull));

  NAPI_CALL(env,
    napi_get_reference_value(env, bTreeWrap->comparator, &comparator));
  napi_value argv[] = {keyA, keyB};

  NAPI_CALL(env,
    napi_call_function(env, esNull, comparator, 2, argv, &esResult));

  NAPI_CALL(env,
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
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Native call to glib tree
  gint nativeHeight = g_tree_height(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env,
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
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Native call to glib tree
  gint nativeSize = g_tree_nnodes(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env,
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
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env,
      napi_throw_error(env, NULL, "Too few arguments."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Create es search box object for comparing with stored in bTree boxes
  NAPI_CALL(env,
    napi_create_object(env, &searchBox));

  NAPI_CALL(env,
    napi_set_named_property(env, searchBox, "key", argv[0]));

  NAPI_CALL(env,
    napi_create_reference(env, searchBox, 0, &searchBoxRef));

  gboolean found = FALSE;

  // Find es value in bTree
  napi_ref targetRef = g_tree_lookup(bTree->nativeTree, searchBoxRef);

  if (targetRef != NULL) {
    found = TRUE;

    // Remove bTree node
    removeTreeNodeTyped(targetRef, bTree);
  }

  NAPI_CALL(env,
    napi_get_boolean(env, found, &result));

  return result;
}

/**
 * ES callback. Add element to bTree.
 */
static napi_value esSet(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTree_t *bTree;
  size_t argc = 2;
  napi_value argv[2], key, value, box;

  // Get es this
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 2) {
    NAPI_CALL(env,
      napi_throw_error(env, NULL, "Expected two arguments."));
  }

  key = argv[0];
  value = argv[1];

  // Extract native BTree pointer
  NAPI_CALL(env,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Create box es object
  NAPI_CALL(env,
    napi_create_object(env, &box));

  // Set key & value to box
  NAPI_CALL(env,
    napi_set_named_property(env, box, "key", key));
  NAPI_CALL(env,
    napi_set_named_property(env, box, "value", value));


  // Set ref counter to 1 for protect from GC
  napi_ref boxRef;
  NAPI_CALL(env,
    napi_create_reference(env, box, 1, &boxRef));

  // Add es plain object to native bTree
  g_tree_replace(bTree->nativeTree, boxRef, boxRef);

  return esThis;
}

/**
 * ES callback. Delete all nodes from bTree
 */
static napi_value esClear(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTree_t *bTree;

  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env,
    napi_unwrap(env, esThis, (void **) &bTree));

  g_tree_foreach(bTree->nativeTree, removeTreeNode, bTree);

  napi_value result;
  NAPI_CALL(env,
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
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env,
      napi_throw_error(env, NULL, "Too few arguments."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env,
    napi_unwrap(env, esThis, (void **) &bTree));

  NAPI_CALL(env,
    napi_create_object(env, &box));

  NAPI_CALL(env,
    napi_set_named_property(env, box, "key", argv[0]));

  NAPI_CALL(env,
    napi_create_reference(env, box, 0, &boxRef));

  gpointer found = g_tree_lookup(bTree->nativeTree, boxRef);

  napi_value result;

  if (found == NULL) {
    NAPI_CALL(env,
      napi_get_boolean(env, false, &result));
  }
  else {
    NAPI_CALL(env,
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
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env,
      napi_throw_error(env, "10", "Expected one argument."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env,
    napi_unwrap(env, esThis, (void **) &bTree));

  napi_value key = argv[0];
  napi_value lookupBox;
  napi_ref lookupRef;

  NAPI_CALL(env,
    napi_create_object(env, &lookupBox));

  NAPI_CALL(env,
    napi_set_named_property(env, lookupBox, "key", key));

  NAPI_CALL(env,
    napi_create_reference(env, lookupBox, 0, &lookupRef));

  // Native call to glib tree
  napi_ref lookupResult = (napi_ref) g_tree_lookup(bTree->nativeTree, lookupRef);

  if (lookupResult == NULL) {
    NAPI_CALL(env,
      napi_get_undefined(env, &result));
  }
  else {
    NAPI_CALL(env,
      napi_get_reference_value(env, lookupResult, &result));

    NAPI_CALL(env,
      napi_get_named_property(env, result, "value", &result));
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
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, (void **) &resultCb));

  // Extract native pointer
  NAPI_CALL(env,
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
  NAPI_CALL(env,
    napi_create_object(env, &esIteratorResult));

  if (itCtxt->state == ITERATOR_END) {
    NAPI_CALL(env,
      napi_get_undefined(env, &itCtxt->value));

    NAPI_CALL(env,
      napi_get_boolean(env, true, &isDone));
  }
  else {
    resultCb(itCtxt);
    NAPI_CALL(env,
      napi_get_boolean(env, false, &isDone));
  }

  NAPI_CALL(env,
    napi_set_named_property(env, esIteratorResult, "value", itCtxt->value));

  NAPI_CALL(env,
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
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, &data));

  // Extract native BTree pointer
  NAPI_CALL(env,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Create es Iterator
  NAPI_CALL(env,
    napi_create_object(env, &esIterator));

  // Create next() iterator method
  napi_value nextFunction;
  NAPI_CALL(env,
    napi_create_function(env, "next", NAPI_AUTO_LENGTH, esIteratorNext, data, &nextFunction));
  NAPI_CALL(env,
    napi_set_named_property(env, esIterator, "next", nextFunction));

  // Alloc memory for native iterator context
  IteratorContext_t *itCtxt = g_new(IteratorContext_t, 1);
  itCtxt->bTree = bTree;
  itCtxt->state = ITERATOR_INIT;

  // Attach native data (context) to es value (iterator)
  napi_ref ref;
  NAPI_CALL(env,
    napi_wrap(env, esIterator, (void *) itCtxt, freeIterator, NULL, &ref));

  return esIterator;
}

/**
 * Native forEach() callback
 */
static gboolean nativeBTreeTraverse(gpointer key, gpointer val, gpointer data) {
  napi_ref objectRef = (napi_ref) val;
  napi_value esObject, esKey, esValue, esNull;
  TraverseData_t *tData = (TraverseData_t *) data;
  napi_env env = tData->bTree->env;

  NAPI_CALL(env,
    napi_get_reference_value(env, objectRef, &esObject));

  NAPI_CALL(env,
    napi_get_named_property(env, esObject, "key", &esKey));

  NAPI_CALL(env,
    napi_get_named_property(env, esObject, "value", &esValue));

  napi_value argv[] = { esKey, esValue };
  NAPI_CALL(env,
    napi_get_null(env, &esNull));

  NAPI_CALL(env,
    napi_call_function(env, esNull, tData->esCallback, 2, argv, NULL));

  return FALSE;
}

/**
 * ES callback. es forEach() method
 */
static napi_value esForeach(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, undefined;
  BTree_t *bTree;
  size_t argc = 1;
  napi_value argv[1];


  // Get es this for current btree
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env,
      napi_throw_error(env, NULL, "Too few arguments."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env,
    napi_unwrap(env, esThis, (void **) &bTree));

  TraverseData_t traverseData = {
    bTree,
    argv[0] // es Callback
  };

  g_tree_foreach(bTree->nativeTree, nativeBTreeTraverse, &traverseData);

  NAPI_CALL(env,
    napi_get_undefined(env, &undefined));

  return undefined;
}

/**
 * ES callback. Constructor
 */
static napi_value esConstructor(napi_env env, napi_callback_info cbInfo) {
  napi_value esBtree;
  napi_ref ref;

  size_t argc = 1;
  napi_value argv[1];

  // Get es arguments & context
  NAPI_CALL(env,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esBtree, NULL));

  // Allocate memory for usre data wich recived in native comparator
  BTree_t *bTree = g_new(BTree_t, 1);

  // Initialize native BTree with native comparator & additional user data
  GTree *nativeTree = g_tree_new_with_data(nativeComparator, bTree);

  // Check type of first argument. Must be function
  napi_valuetype comparatorType;
  NAPI_CALL(env,
    napi_typeof(env, argv[0], &comparatorType));

  if (comparatorType != napi_function) {
    NAPI_CALL(env,
      napi_throw_error(env, "10", "First arg must be comparator qsort() like function"));
  }

  // Fill user data
  bTree->nativeTree = nativeTree;
  bTree->env = env;

  // Create ref on comparator function. Protect from GC
  NAPI_CALL(env,
    napi_create_reference(env, argv[0], 1, &bTree->comparator));

  // Wrap native data in ES variable for native access again
  NAPI_CALL(env,
    napi_wrap(env, esBtree, bTree, freeNativeBTree, NULL, &ref));

  return esBtree;
}

/**
 * Module initialization callback
 */
static napi_value init(napi_env env, napi_value exports) {
  napi_value esBTreeClass, symbolIterator;

  NAPI_GLOBAL_SYM(env, "iterator", symbolIterator);

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
    }
  };

  NAPI_CALL(env,
    napi_define_class(env, "BTree", NAPI_AUTO_LENGTH, esConstructor, NULL, (sizeof(esBTreeProps) / sizeof(esBTreeProps[0])), esBTreeProps, &esBTreeClass));

  NAPI_CALL(env,
    napi_create_reference(env, esBTreeClass, 1, &constructor));

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

  NAPI_CALL(env,
    napi_define_properties(env, exports, (sizeof(props) / sizeof(props[0])), props));

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
