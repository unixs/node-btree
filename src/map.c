#include <common.h>


/**
 * Unref all bTree nodes for GC access.
 */
static gboolean removeTreeNode(gpointer key, gpointer val, gpointer data) {
  BTree_t *bTree = (BTree_t *) data;

  // Remove es value from native bTree
  g_tree_remove(bTree->nativeTree, (BTreeNode) key);

  return FALSE;
}


/**
 * ES callback. Return value from bTree by key
 */
napi_value esGet(napi_env env, napi_callback_info cbInfo) {
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
napi_value esDelete(napi_env env, napi_callback_info cbInfo) {
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
napi_value esSet(napi_env env, napi_callback_info cbInfo) {
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
napi_value esSize(napi_env env, napi_callback_info cbInfo) {
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
napi_value esClear(napi_env env, napi_callback_info cbInfo) {
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
napi_value esHas(napi_env env, napi_callback_info cbInfo) {
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
