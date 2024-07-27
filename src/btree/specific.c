#include <core.h>
#include <interface.h>


/**
 * ES callback. Return bTree height
 */
napi_value
esHeight(napi_env env, napi_callback_info cbInfo)
{
  napi_value esHeight;
  napi_value esThis;
  BTree_t *bTree;

  // Get es this
  NAPI_CALL(env, false,
            napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, false, napi_unwrap(env, esThis, (void **) &bTree));

  // Native call to glib tree
  gint nativeHeight = g_tree_height(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env, false, napi_create_int64(env, nativeHeight, &esHeight));

  return esHeight;
}

napi_value
esForeachReverse(napi_env env, napi_callback_info cbInfo)
{
  napi_value esThis, callback, cbThis, cbArgv[4], argv[2];
  size_t argc = 2;

  NAPI_CALL(env, false,
            napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  CHECK_ARGC(1, msgTooFewArguments);
  callback = argv[0];

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  if (argc > 1) {
    cbThis = argv[1];
  }
  else {
    cbThis = getEsGlobal(env);
  }

#ifdef HAS_GTREE_NODE
  GTreeNode *current = g_tree_node_last(btree->nativeTree);

  gint idx = 0;
  gint revIdx = g_tree_nnodes(btree->nativeTree);

  while (current) {
    cbArgv[0] = getNodeEsValue(env, current);
    cbArgv[1] = getNodeEsKey(env, current);

    NAPI_CALL(env, false, napi_create_int64(env, idx, &cbArgv[2]));

    idx++;
    revIdx--;

    NAPI_CALL(env, false, napi_create_int64(env, revIdx, &cbArgv[3]));

    NAPI_CALL(env, false,
              napi_call_function(env, cbThis, callback,
                                 (sizeof(cbArgv) / sizeof(napi_value)), cbArgv,
                                 NULL));

    current = g_tree_node_previous(current);
  }
#else
  GPtrArray *arr = gtreeToPtrArray(btree->nativeTree);

  napi_value key, value;
  napi_ref item;
  for (glong revIdx = arr->len - 1, idx = 0; revIdx >= 0; revIdx--, idx++) {
    item = (napi_ref) g_ptr_array_index(arr, revIdx);

    unrefBtreeNodeEsObject(env, item, &key, &value);

    cbArgv[0] = value;
    cbArgv[1] = key;

    NAPI_CALL(env, false, napi_create_int64(env, idx, &cbArgv[2]));

    NAPI_CALL(env, false, napi_create_int64(env, revIdx, &cbArgv[3]));

    NAPI_CALL(env, false,
              napi_call_function(env, cbThis, callback,
                                 (sizeof(cbArgv) / sizeof(napi_value)), cbArgv,
                                 NULL));
  }

  g_ptr_array_free(arr, TRUE);
#endif

  return getEsUndef(env);
}

napi_value
esToStringTag(napi_env env, napi_callback_info cbInfo)
{
  napi_value result;

  NAPI_CALL(
      env, false,
      napi_create_string_utf8(env, ES_CLASS_NAME, NAPI_AUTO_LENGTH, &result));

  return result;
}
