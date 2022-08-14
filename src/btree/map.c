#include <core.h>
#include <foreach.h>
#include <interface.h>


/**
 * Unref all bTree nodes for GC access.
 */
static gboolean
removeTreeNode(gpointer key, gpointer val, gpointer data) {
  BTree_t *bTree = (BTree_t *) data;

  // Remove es value from native bTree
  g_tree_remove(bTree->nativeTree, (BTreeNode) key);

  return FALSE;
}


/**
 * Native forEach() callback
 */
static gboolean
nativeBTreeForEach(gpointer key, gpointer val, gpointer data) {
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
 * ES callback. Return value from bTree by key
 */
napi_value
esGet(napi_env env, napi_callback_info cbInfo) {
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
 * ES callback. Delete node from bTree by key
 */
napi_value
esDelete(napi_env env, napi_callback_info cbInfo) {
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
napi_value
esSet(napi_env env, napi_callback_info cbInfo) {
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
 * ES callback. Return bTree size (nodes count).
 */
napi_value
esSize(napi_env env, napi_callback_info cbInfo) {
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
 * ES callback. Delete all nodes from bTree
 */
napi_value
esClear(napi_env env, napi_callback_info cbInfo) {
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
napi_value
esHas(napi_env env, napi_callback_info cbInfo) {
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
 * ES callback. es forEach() method
 */
napi_value
esForeach(napi_env env, napi_callback_info cbInfo) {
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
