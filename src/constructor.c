#include "constructor.h"

napi_ref constructor;

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

static void freeTreeValue(gpointer treeValue) {
  FREE_NODE(treeValue);
}

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
 * ES callback. Constructor
 */
napi_value esConstructor(napi_env env, napi_callback_info cbInfo) {
  napi_value esBtree, comparator, isConstructor;

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
      napi_wrap(env, esBtree, bTree, freeNativeBTree, NULL, NULL));
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
